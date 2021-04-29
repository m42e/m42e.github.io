---
title: What is Most Important when Porting Legacy Software?
published: true
---

I am currently working on a code base which is rather old an used to be `C`. It has been started over a decade ago and is still in use. As part of an automotive ECU it has seen a number of base systems it has been integrated to. If you are familiar with automotive software development you may know `AUTOSAR`, a kind of extra layer above the OS which should make software components portable between and independent of the actual chip or event ECU it is running on.

So the software we are currently porting fulfills the requirements and is bound to the constraints `AUTOSAR` requires. Additionally, as it has existed before, it has even more legacy interfaces. 

The target is to port things to a modern POSIX compliant platform. And to make it more modern, to C++. C++17 to be a bit more precise. This is a great opportunity in my point of view. Get rid of all the old things that had to be supported, clean up interfaces and use the standard library. But most important: make the code more readable, understandable and maintainable.

To achieve these three things I currently see one thing as crucial: **Make the code descriptive**. Well, to not just give the thing another name, let me explain. I would define descriptiveness as a metric of how many lines of code do i have to read (or even worse, scroll) to determine the meaning of a single line of code.

Given the following function `pt1filter`. Lets assume it is everything in one file. Old `C` style. 

```C
void PT1Filter(uint32* pOldValue, uint32 NewValue, uint16 DeltaT,
               uint16 TimeConstant) {
  uint32 DeltaV;

  if (TimeConstant > 0U) {
    if (TimeConstant < DeltaT) {
      *pOldValue = NewValue;
    } else {
      if (NewValue > *pOldValue) {
        DeltaV = (NewValue - *pOldValue) * (uint32)DeltaT;
        DeltaV += (uint32)TimeConstant - 1uL;
        DeltaV /= (uint32)TimeConstant;
        *pOldValue += DeltaV;
      } else {
        DeltaV = (*pOldValue - NewValue) * (uint32)DeltaT;
        DeltaV += (uint32)TimeConstant / 2uL;
        DeltaV /= (uint32)TimeConstant;

        if ((NewValue == 0uL) && (*pOldValue != 0uL) && (DeltaV == 0uL)) {
          DeltaV = 1;
        }

        if (*pOldValue > DeltaV) {
          *pOldValue -= DeltaV;
        } else {
          *pOldValue = 0;
        }
      }
    }
  } else {
    *pOldValue = NewValue;
  }
}
```

Some design considerations have been done because of the limitation of previous platforms, e.g. limited use of float or memory restrictions.

Now I had somebody porting it to `C++`. Now we have the following. Well it compiles well with C++, looks a bit modern as it is now using templates, but that's it. There has been an additional tests, for an upper and lower limit. It is not better readable nor understandable than before. And it now has **six** parameters.

```CPP
template <typename T>
inline T pt1Filter(const T oldValue, const T newValue, const T upperLimit,
                   const T lowerLimit, const T deltaT, const T timeConstant) {
  T deltaV;
  bool filteringEnabled{true};
  if (newValue >= oldValue) {
    deltaV = static_cast<T>(newValue - oldValue);
    if (deltaV > upperLimit) {
      filteringEnabled = false;
    }
  } else {
    deltaV = static_cast<T>(oldValue - newValue);
    if (deltaV > lowerLimit) {
      filteringEnabled = false;
    }
  }

  T result;
  if ((filteringEnabled == true) && (deltaV > 0) && (deltaT < timeConstant) &&
      (timeConstant > 0)) {
    const float numerator{
        static_cast<float>((oldValue * deltaT) + (newValue * timeConstant))};
    const float denominator{static_cast<float>((deltaT + timeConstant))};
    result = static_cast<T>(
        std::floor((numerator + (denominator / 2.0F)) / denominator));
  } else {
    result = newValue;
  }

  return result;
}
```

If we choose any of the lines, we might have a clue, what's going on here, but it is hard to get to it right away, are we?

Lets make it a bit simpler.

```CPP
template <typename T>
class Limits {
 public:
  T const lower;
  T const upper;

  bool isWithin(T value) const { return (value <= upper) && (value >= lower); }
};

template <typename T>
typename std::enable_if_t<std::is_integral_v<T>, T> pt1Filter(
    T const old_value, T const new_value, Limits<T> const& limits,
    std::chrono::milliseconds const& time_delta,
    std::chrono::milliseconds const& time_constant) {
  auto const value_delta{new_value - old_value};

  if (!limits.isWithin(value_delta) || time_constant.count() <= 0 ||
      time_delta >= time_constant || std::abs(value_delta) == 0) {
    return new_value;
  }

  auto const numerator{static_cast<float>((old_value * time_delta.count()) +
                                          (new_value * time_constant.count()))};
  auto const denominator{
      static_cast<float>((time_delta.count() + time_constant.count()))};
  return static_cast<T>(std::lround(numerator / denominator));
}
```

So from my point of view it is a bit better. Basically I changed the following aspects:

- use standard and meaningful types,
- create a type, where it made sense,
- return early.

I am not satisfied with this, as 5 parameters are likely to get wrong. To make it simpler. First thing is to check how it is actually called.

```CPP
const uint32_t upperLimitPt1{50};
const uint32_t lowerLimitPt1{0};
....
filtered = pt1Filter(new, old, {lowerLimitPt1, upperLimitPt1}, deltaT, tconst);
```


What we can see here, is that the limits are set to fixed values by the caller. That should be a hint for us, to do is in another way. There is a part that changes `tconst` but this is barely called. So maybe we can do better, and work with the limits and the time constant as kind of configuration.

The limits are really a static configuration of the filter, the `tconst` is a dynamic configuration. And so I decided to put the filter into a class and take the configuration as part of the constructor:

```CPP
Pt1Filter(Limits<T> const& limits, 
         std::chrono::milliseconds const& time_constant = std::chrono::milliseconds(0)) 
         : limits_{limits}, time_constant_{time_constant_} {}
```

As we want to be able to change the time constant at some time during the runtime, we add a `setTimeConstant` interface. The rest of the parameters stays the same and is moved into `operator()` member.

```CPP
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
class Pt1Filter {
 public:
  Pt1Filter(Limits<T> const& limits,
            std::chrono::milliseconds const& time_constant =
                std::chrono::milliseconds(0))
      : limits_{limits}, time_constant_{time_constant_} {}
  void setTimeConstant(std::chrono::milliseconds const& time_constant) {
    time_constant_ = time_constant_;
  }

  T operator()(T const old_value, T const new_value,
               std::chrono::milliseconds const& time_delta) const {
    auto const value_delta{new_value - old_value};

    if (!limits_.isWithin(value_delta) || time_constant_.count() <= 0 ||
        time_delta >= time_constant_ || isValidDelta(value_delta)) {
      return new_value;
    }

    auto const numerator{
        static_cast<float>((old_value * time_delta.count()) +
                           (new_value * time_constant_.count()))};
    auto const denominator{
        static_cast<float>((time_delta.count() + time_constant_.count()))};
    return static_cast<T>(std::lround(numerator / denominator));
  }

 private:
  Limits<T> const& limits_;
  std::chrono::milliseconds time_constant_;

  inline bool isValidDelta(T delta) const { return std::abs(delta) != 0; }
};
```

Now lets reduce the functions parameters.

So what changes did I actually made.

Used types that represent what I actually care about. In this example I changed the type of the parameter which represents a certain amount of time to `std::chrono::duration`. The algorithm makes use of some limits. Making the upper and lower limit part of a `struct` makes the signature easier to read, understand and harder to pass the parameters in the incorrect order. And finally, I removed the parameters which are actual `const` anyway in the callers context and passed in every time. What we get is a lean interface and using standard library also a clean implementation which is (nearly) readable by non coders.

## [](#tldr)TL;DR

If you ever porting legacy code consider the following:

- understand the intention
- use proper, expressive types (use existing or create new ones)
- know the std algorithms
- try to make the interfaces as small as possible


## [](#extra)Extra

How to ensure it really works? There are no tests available for the legacy implementation? You are lazy?
I did it like that:

- move the old implementation to the test code
- wrote a data driven testcase, with a lot of combinations, taken from the code, utilizing `testing::Combine`. The test only compares the original result with the one of the reimplementation.
- did the reimplementation

I had about 3.5 million tests to verify my solution actually works. It is not the kind of test you may want to run every time. It may also break your test result reporting tool. 
But while implementing such a replacement I found it very helpful and pleasing to know it actually works.


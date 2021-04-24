---
title: What is Most Important when Porting Legacy Software?
published: false
---

I am currently working on a code base which is rather old an used to be `C`. It has been started over a decade ago and is still in use. As part of an automotive ECU it has seen a number of base systems it has been integrated to. If you are familiar with automotive software development you may know `AUTOSAR`, a kind of extra layer above the OS which should make software components portable between and independent of the actual chip or event ECU it is running on.

So the software we are currently porting fulfills the requirements and is bound to the constraints `AUTOSAR` requires. Additionally, as it has existed before, it has even more legacy interfaces. 

The target is to port things to a modern POSIX compliant platform. And to make it more modern, to C++. C++17 to be a bit more precise. This is a great opportunity in my point of view. Get rid of all the old things that had to be supported, clean up interfaces and use the standard library. But most important: make the code more readable, understandable and maintainable.

To achieve these three things I currently see one thing as crucial: **Make the code descriptive**. Well, to not just give the thing another name, let me explain. I would define descriptiveness as a metric of how many lines of code do i have to read (or even worse, scroll) to determine the meaning of a single line of code.

Given the following function `pt1filter`. Lets assume it is everything in one file. Old `C` style. 

```C
```

Some design considerations have been done because of the limitation of previous platforms, e.g. limited use of float.

Now I had somebody porting it to `C++` that is the outcome. Well it now compiles well with C++, looks a bit modern as it is now using templates, but that's it. It is no more readable or anything else. 

```CPP
```

If we choose any of the lines, we might have a clue, what's going on here, but it is hard to get to it right away.

Lets make it a bit simpler.

```CPP
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


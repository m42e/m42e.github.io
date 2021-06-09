---
title: Ignore the nodiscard
published: true
---

I have recently decided to have `[[nodiscard]]` for every function that returns something. Every may have some exceptions but in general all normal every-day functions are defined `[[nodiscard]]`.

This has two reasons:

- If there is a return value, it should mean something, so you should actively decide what to do.
- If the return value is not useful at all, remove it.

## Non owned code

So far so good, the rule is nice and works well, but in our code base we have to deal with lots of third party code we cannot change, at least not easily. I think we all know the struggle.
Additionally to the compiler raising errors on each ignored return value flagged with `[[nodiscard]]` we have some static code checkers which perform this check for all the function calls.

We have a third party interface which has a return value, but it is not useful for us. So we want to ignore it. Simply discard it with not at least some cast does not work, as the mentioned code checker would flag it as an issue.

Thankfully we get something within the `std` even if it is not exactly where you might expect it. You have to `#include <tuple>`. And then you get a small but useful gem. `std::ignore`. 
Remember the old days, where you did something like `(void) function_with_return()` or a more C++ style `static_cast<void>(function_with_return())`. These times are gone. From now on you can simply state what you want to do:

```cpp
#include <tuple>

int function_with_return(){
  return 54;
}

int main(){
  std::ignore = function_with_return();
  return 0;
}
```

This makes it pretty clear you are not interested in the value receiving as a return. I like it pretty much as it solves another inconvenience we dealt way to long.

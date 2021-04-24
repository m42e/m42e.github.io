---
title: What are Inline Namespaces?
published: true
---

Inline namespaces are transparent for those who are using the outer namespace.
This is useful if you want to allow to narrow down the functions/classes/constants used, but do not want to add some more clutter if somebody wants to have them all.

A simple example:

## [](#example)Simple Example

```cpp
#include <iostream>
namespace m42e {
namespace cpp {
namespace test {
const int32_t my42 = 42;
inline namespace nested {
const int32_t my23 = 23;
}  // namespace nested
}  // namespace test
}  // namespace cpp
}  // namespace m42e

int main() { 
  // this works for sure
  std::cout << m42e::cpp::test::my42 << std::endl;
  std::cout << m42e::cpp::test::nested::my23 << std::endl;
  {
    using namespace m42e::cpp::test;
    // of course we can use my42 here
    std::cout << my42 << std::endl;
    // as nested is inline it is "transparent"
    std::cout << my23 << std::endl;
  }
  
  return 0; 
}
```

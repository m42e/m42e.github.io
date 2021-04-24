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
  std::cout << m42e::cpp::test::my42 << std::endl;
  std::cout << m42e::cpp::test::nested::my23 << std::endl;
  {
    using namespace m42e::cpp::test;
    // as nested is inline, it is somehow "transparent"
    std::cout << my42 << std::endl;
    std::cout << my23 << std::endl;
  }
  return 0;
}

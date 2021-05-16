#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <ratio>
#include <vector>

class WrapperBase {
 public:
  virtual bool isGoodValue() = 0;
  virtual explicit operator int() const = 0;
};

template <typename T>
class WrapperSolveEverything : public WrapperBase {
 public:
  WrapperSolveEverything<T>(T value) : value_{value} {}
  bool isGoodValue() override {
    return std::rand() >= 10;
  }

  operator int() const override { return static_cast<int>(std::ceil(value_));}

 private:
  T value_;
};

class WeAllLoveManagers {
 public:
  template <typename T>
  static std::shared_ptr<WrapperBase> generateMeOne(T value) {
    return std::make_shared<WrapperSolveEverything<T>>(value);
  }
  template <typename... Ts>
  static bool valid(Ts... args) {
    return (args->isGoodValue() && ...);
  }
  template <typename... Ts>
  static int sum(Ts... args) {
    return static_cast<int>((static_cast<int>(*args) + ...));
  }
};

int main() {
  auto x = WeAllLoveManagers::generateMeOne<float>(12.5);
  auto y = WeAllLoveManagers::generateMeOne<uint8_t>(12);
  auto z = WeAllLoveManagers::generateMeOne<int>(-12);
  std::cout << "valid? " << WeAllLoveManagers::valid(x, y, z) << std::endl;
  std::cout << "sum? " << WeAllLoveManagers::sum(x, y, z) << std::endl;
  return 0;
}

#include <string>
#include <variant>
#include <iostream>

using namespace std::literals;

void a(std::variant<int, std::string>& data){
  std::cout << "size is " << sizeof(data) << std::endl;
  std::visit([](auto&& arg){
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, int>){
        std::cout << "int " << arg << '\n';
    }else if constexpr (std::is_same_v<T, std::string>){
        std::cout << "string " << arg << '\n';
    }
  }, data);
}


int main(){
  std::variant<int, std::string> s = "AString"s;
  std::variant<int, std::string> z = 3208;

  a(s);
  a(z);

  return 0;
}

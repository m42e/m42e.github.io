#include <string>
#include <variant>
#include <iostream>

using namespace std::literals;

void funcb(std::variant<int, std::string>& data){
  if(auto pval = std::get_if<std::string>(&data)){
    std::cout << "a string, how nice: " << pval << std::endl;
  }
  else{
    std::cout << "not a string, something else" << std::endl;
  }
}

void a(std::variant<int, std::string>& data){
  std::cout << "size is " << sizeof(data) << std::endl;
  funcb(data);
}


int main(){
  std::variant<int, std::string> s = "AString"s;
  std::variant<int, std::string> z = 3208;

  a(s);
  a(z);

  return 0;
}

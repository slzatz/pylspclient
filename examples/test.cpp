#include <iostream>
#include <string>
#include <vector>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
/* 

push_back
clear
insert
erase

*/

struct A {
   std::string s;
   int x;
   A(std::string S, int X) : s(S), x(X) {};
};

int main() {

  A a0{"hello", 5};
  A a1{"goodbye", 100};
  A a2{"norm", 50};
  
  std::vector<A> v;
  v.push_back(a0);
  v.push_back(a1);
  v.push_back(a2);
  
  int y = 5;

  int id = 100;
  //auto it = std::find_if(v.begin(), v.end(), [&id](auto n){return n.x == id;});

  //std::cout << it->s << std::endl;
  //fmt::print("You selected {}", it->s);
  fmt::print("Hello {}", "Steve");

return 0;
}
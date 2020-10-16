#include <iostream>
#include <string>
#include <map>
#include <vector>

/* some common string operations
should include string_view*/

int main()  {

  std::string s("Bodie is a Good Boy");
  std::cout << s << std::endl;
  s.insert(7,"Girl" );
  std::cout << s << std::endl;
  s.append("Norm");
  std::cout << s << std::endl;
  s.erase(4,8);
  std::cout << s << std::endl;
  
  std::map<int,int> m{{1,2}, {3,4}};
  
  //if (m.contains(1)) std::cout << "yes 1" << std::endl;
  if (m.count(1)) std::cout << "yes 1" << std::endl;

  else std::cout << "no 1" << std::endl;
  //if (m.contains(4)) std::cout << "yes 4" << std::endl;
  if (m.count(4)) std::cout << "yes 4" << std::endl;
  else std::cout << "no 4" << std::endl;
  
  std::string zz = "helloo";
  ////
  
  std::vector<int> v{1,2,3,4};
  v.pushback(5);
  
  return 1;
}
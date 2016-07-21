#include "contentStructure.h"

int main() {
   ContentStructure s;
   s.addContent("hi", 0);
   s.addContent("lo", 1);
   s.addContent("fi", 2);
   s.addContent("wi", 3);
   s.addContent("ai", 4);
   s.addContent("di", 5);
   s.addContent("fi", 6);
   std::cout << "adds complete" << std::endl;

   std::string t;

   t = s.lookupContent(3);
   if (t != "") std::cout << t << std::endl;
   else std::cerr << "Error: no such content" << std::endl; //should output "wi"

   t = s.lookupContent(9);
   if (t != "") std::cout << t << std::endl;
   else std::cerr << "Error: no such content" << std::endl; //should error

   if (s.removeContent(3) != 0) std::cerr << "Error: no such content" << std::endl; //should work silently
   if (s.removeContent(9) != 0) std::cerr << "Error: no such content" << std::endl; //should error
}

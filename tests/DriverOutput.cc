#include <iostream>
#include <fstream>
#include "../include/Output.hh"

int main()
{
   std::ifstream is("Tiny.json");
   nlohmann::json jsonfile;

   is >> jsonfile;

   Input network(jsonfile);

   Output solution(network);

   std::cout << network <<std::endl<<solution <<std::endl;
   return 0;
}

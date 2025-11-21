#include <fstream>
#include "../include/Input.hh"
#include <iostream>

int main()
{
    std::ifstream is("Tiny.json");
    nlohmann::json jsonfile;

    is >> jsonfile;

    Input network(jsonfile);

    std::cout << network <<std::endl;
    return 0;
}

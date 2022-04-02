#include <fstream>
#include "network.hpp"

int main(){
    std::ifstream is;
    nlohmann::json s;

    is.open("Tiny.json");
    is >> s;
    for (auto rel: s["cell_relations"] )
    {
        CellRelation test(rel);
        std::cout << "from" << rel["from"] << std::endl;
        std::cout << test << std::endl;
    }
    
    return 0;
}
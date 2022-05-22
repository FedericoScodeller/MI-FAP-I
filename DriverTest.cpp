#include <fstream>
#include "network.hpp"

int main(){
    std::ifstream is;
    std::ofstream out;
    nlohmann::json s;
    

    out.open("Tiny.txt");
    
    is.open("Tiny.json");
    is >> s;
    
    Network net(s);
    
    net.AddTransmitters(s["cells"]);
    net.AddRelations(s["cell_relations"]);

    //test largest degree mk1

    net.AssignFreq_mk2();

    out << net;
    out.close();
    is.close();
    return 0;
}
#include <fstream>
#include "network.hpp"

int main(int argc, char* argv[])
{
    std::ifstream data("data");
    std::ifstream is;

    std::ofstream out("result");

    std::string file_name;
    nlohmann::json jsonfile;

    while (data)
    {
        data >> file_name;
        is.open(file_name);
        is >> jsonfile;
        Network net(jsonfile);
        std::cout << "working on " << file_name << std::endl;
        net.MiFap("Greedy");
        out << file_name << " " << net;
        is.close();
    }
    out.close();
    data.close();
    std::cout << "Eseguibile concluso" << std::endl;
    return 0;
}


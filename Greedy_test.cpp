#include <fstream>
#include "network.hpp"

int main(int argc, char* argv[])
{
    std::ifstream is;
    std::ofstream out;
    nlohmann::json s;
    std:: string substring = ".json";
    std:: string str;
    if (argc != 2)
    {
        std::cout << "il programma richiede in linea di comando il nome del file json su cui operare"<< std::endl;
        return EXIT_FAILURE;
    }
    str=argv[1];
    
    is.open(str);
    is >> s;
    
    std::size_t ind = str.find(substring); 
    if(ind !=std::string::npos){
        str.erase(ind,substring.length()); 
    }
    out.open(str+"_greedy.txt");

    Network net(s);
    net.Greedy();

    out << net;
    out.close();
    is.close();
    std::cout << "Tot interfernza: "<< net.TotInterference() << std::endl;
    std::cout << "Eseguibile concluso" << std::endl;
    return 0;
}
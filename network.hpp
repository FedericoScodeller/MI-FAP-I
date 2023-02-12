#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "json.hpp"
#include "transmitter.hpp"

/*questa è la classe principale che contiene tutte le  principali informazioni generali e metodi per gestire
il problema*/
//std::cout <<"\r" <<n+1<<"/"<< data["cell_relations"].size() << " cell relation"<< std::flush; 
struct FreqCost
{
    int hard_link = 0;
    float interference = 0.0;
};

class Network
{
    friend std::ostream& operator<<(std::ostream& os, const Network& N);
private:

    int min_freq;
    int max_freq;
    std::vector<int> globally_blocked_channels;

    int co_site_separetion;
    int co_cell_separetion;

    int bcch_bcch;
    int bcch_tch;
    int tch_bcch;
    int tch_tch;

/*tutte le celle sono composte da 1 o più trasmettitori i quali sono la minima unita da gestire*/
    std::vector<Transmitter> transmitters;

    std::vector<std::vector<int>> separetion;
    std::vector<std::vector<float>> adjacent;
    std::vector<std::vector<float>> same;

    std::vector<std::vector<int>> FindTx(std::string from,std::string to);
    int HandoverCost(int from, int to);
    struct FreqCost FreqCost(int tx, int freq); 
    void AssignFreq(int tx);
    int Degree(int transmettitore);
    int Saturation(int transmettitore);
    int LargestDegree(void);
    int DSatur(void);

public:

    Network(nlohmann::json data);
    void MiFap(std::string algoritmo);
};
#endif

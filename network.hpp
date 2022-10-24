#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <iostream>
#include <vector>
#include <string>
#include "json.hpp"
#include "relation.hpp"
#include "transmitter.hpp"

/*questa è la classe principale che contiene tutte le  principali informazioni generali e metodi per gestire
il problema*/

class Network
{
    friend std::ostream& operator<<(std::ostream& os, const Network& N);
private:
/*l'intervallo di freq*/     
    int min_freq;
    int max_freq;
/*le separazioni per i vari casi*/
    int co_site_separetion;
    int co_cell_separetion;
/*separazioni specifiche per i casi con handover*/
    int bcch_bcch;
    int bcch_tch;
    int tch_bcch;
    int tch_tch;
/*i canali bloccati per tutti i trasmettitori, nota che ci sono anche canali bloccati solo per certe celle*/
    std::vector<int> globally_blocked_channels; 
/*tutte le celle sono composte da 1 o più trasmettitori i quali sono la minima unita da gestire*/
    std::vector<Transmitter> transmitters;
public:
    Network(nlohmann::json data);
    int HandoverCost(Transmitter from, Transmitter to);
    void Greedy(void);
    void AssignFreq(int tx, int freq);
    double TotInterference();
};
#endif

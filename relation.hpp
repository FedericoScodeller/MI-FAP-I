#ifndef RELATION_HPP
#define RELATION_HPP

#include <iostream>
#include <vector>
#include <string>
#include "json.hpp"

class Relation
{
    friend std::ostream& operator<<(std::ostream& os, const Relation& r);
private:
    int tx; //the transmitter that cause this constrain
    bool handover; // non ha molti usi se non al inizio forse posso toglierlo
    int separetion;
    double interf_same_ch;
    double interf_adjacent_ch;
public:
/*ho due generatori per due casi diversi di creazione*/
    Relation(int tx_id, int sep);         //used in the cicle for co_cell & co_site relation
    Relation(nlohmann::json cell_rel,int tx_id, int sep);   //used in the cicle for cell_relation from json file 
    int Tx(void) {return tx;};
    bool Handover(void) {return handover;};
    int Separetion(void) {return separetion;};
    double InterfSameCh(void) {return interf_same_ch;};
    double InterfAdjacentCh(void) {return interf_adjacent_ch;};
    void ModifyRel(Relation data);
};


#endif
#ifndef TRANSMITTER_HPP
#define TRANSMITTER_HPP

#include <iostream>
#include <vector>
#include <string>
#include "json.hpp"


enum class transmitter_type {bcch, tch}; 
//ha importanza solo per il calcolo della separazione nelle relazioni con handover

class Transmitter
{
    friend std::ostream& operator<<(std::ostream& os, const Transmitter& t);
private:
/*info generali della cella,nota che essa è identificata dalla sua posizione nel vettore in network*/
    int freq;
    std::string cell;
    std::string site;
    transmitter_type type;
    std::vector<int> blocked_channels;
public:
    Transmitter(nlohmann::json cell_info, transmitter_type channel);
    int Freq() const {return freq;};
    std::string Cell(void) const{ return cell; };
    std::string Site(void) const { return site; };
    transmitter_type Type(void) const { return type; };
    bool BlockedFreq(int f) const {return std::count((*this).blocked_channels.begin(), (*this).blocked_channels.end(), f);};
    void AssignFreq(int f){(*this).freq=f;}
};

#endif
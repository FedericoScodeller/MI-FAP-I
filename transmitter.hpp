#ifndef TRANSMITTER_HPP
#define TRANSMITTER_HPP

#include <iostream>
#include <vector>
#include <string>
#include "json.hpp"
#include "relation.hpp"

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
/*questo tiene conto delle freq bloccate localmente*/
    std::vector<int> blocked_channels;
/*le relazioni in cui il trasmettitore corrente compare in "to" quelle in cui subisce il vincolo
 e quindi questo vettore salva le celle che sono nella voce from del file .json*/
    std::vector<Relation> from_relation; 
/*le relazioni in cui il trasmettitore corrente compare in "from" quelle in cui causa il vincolo
e quindi questo vettore salva le celle che sono nella voce to del file .json, ho solo l'id poichè 
le informazioni che mi servono sono nel vettore from_relation di tale transmettitore*/
    std::vector<int> to_relation; 
public:
    Transmitter(nlohmann::json cell_info, transmitter_type channel);
    int Freq(){return freq;};
    std::string Cell(void){ return cell; };
    std::string Site(void){ return site; };
    transmitter_type Type(void){ return type; };

    void UpdateRel (Relation rel);
    void AddToRelation(int r){(*this).to_relation.push_back(r);};
/*sono funzioni che danno informazioni della presenza o meno di certi valori*/
    bool InToRelation (int r){return std::count((*this).to_relation.begin(), (*this).to_relation.end(), r);};
    bool BlockedFreq(int f){return std::count((*this).blocked_channels.begin(), (*this).blocked_channels.end(), f);};
/*informzioni sulla dimensione per i cilci in Network*/
    std::vector<Transmitter>::size_type FromSize(){return from_relation.size();};
    std::vector<int>::size_type ToSize(){return to_relation.size();};
/*estraggono informazioni dalle relazioni per essere usate dal Network, questo non mi piace troppo
vorre sostituirlo con qualcosa di più elegante/leggibile*/
    int TxFrom(int n){return (*this).from_relation[n].Tx();};
    int TxTo(int n){return (*this).to_relation[n];};
    int FindTxInFrom(int tx);
    double CostFreq(int tx,int tx_freq, int freq);
    void AssignFreq(int f){(*this).freq=f;}
};

#endif
#include "transmitter.hpp"
#include <cmath>

//TRANSMITTOR METHOD AND FRIEND FUNCTION
Transmitter::Transmitter(nlohmann::json cell_info, transmitter_type channel)
{
    freq=-1; 
    cell = cell_info["id"]; 
    site = cell_info["site"]; 
    type = channel;

    if (cell_info["blocked_channels"] != nlohmann::detail::value_t::null)
    {
        for (nlohmann::basic_json<>::size_type i=0; i < cell_info["blocked_channels"].size();i++)
        {
            int j = cell_info["blocked_channels"][i]; 
            blocked_channels.push_back(j);
        }  
    }
}


void Transmitter::UpdateRel(Relation rel)
{
    /*usato per aggiungere eaggiornare una voce del vettore from relation*/
    for (std::vector<Relation>::size_type i = 0; i < (*this).from_relation.size(); i++)
    {
        if ((*this).from_relation[i].Tx()== rel.Tx())
        {
            (*this).from_relation[i].ModifyRel(rel);
            return;
        }
    }
    (*this).from_relation.push_back(rel);
}


int Transmitter::FindTxInFrom(int tx)
{
    for (std::vector<Transmitter>::size_type i = 0; i < (*this).from_relation.size(); i++)
    {
        if ((*this).from_relation[i].Tx()==tx)
        {
            return i;
        }
    }
    return -1;
}


double Transmitter::CostFreq(int tx,int tx_freq,int freq)
{
    double cost=0.0;
    int n=(*this).FindTxInFrom(tx);
    
    if (freq==-1 || tx_freq==-1 || n==-1)
    {
        /*costo è nullo se una delle freq non è inizializzata o se il tx con cui eseguire il
        controllo non è uno che causa interfernze e quindi non è nel vettore from_relation*/
        return 0.0;
    }

    /*dato che come tenere conto del costo di interferenze è stato lasciato alla mia indiscrezione
        ho creato questa formula che viene spiegata meglio nel readme file e da le varie giustificazioni*/
    if ((*this).from_relation[n].Separetion() > abs(freq - tx_freq))
    {
        cost+=1.0+0.5*((*this).from_relation[n].Separetion()-1-abs(freq - tx_freq));
        if ((*this).from_relation[n].InterfSameCh()==0.0)
        {
            cost+=0.5;//erano legami non permettevano rottura come cosite e handover puri, il caso particolare e cocella
        }
    }
    
    if (abs(freq - tx_freq)==1)
    {
        cost+=(*this).from_relation[n].InterfAdjacentCh();
    }

    if (abs(freq - tx_freq)==0)
    {
        cost+=(*this).from_relation[n].InterfSameCh();
    }
    
    return cost;
    
}


std::ostream& operator<<(std::ostream& os, const Transmitter& t)
{
    /*come in network non usalo se non servono informzioni dettagliate*/
    os << "freq: " << t.freq << std::endl;
    os << "id: " << t.cell << std::endl;
    os << "site: " << t.site << std::endl;

    if (t.type == transmitter_type::bcch)
        os << "type: bcch" << std::endl;
    else
        os << "type: tch" << std::endl;

    os << "blocked_channels: [";
    for (std::vector<Relation>::size_type i = 0; i < t.blocked_channels.size(); i++)
        os << t.blocked_channels[i]<< ", ";
    os << "]" << std::endl<< std::endl<< std::endl;

    os << "to_relation [";
    for (std::vector<int>::size_type i = 0; i < t.to_relation.size(); i++)
        os << t.to_relation[i]<< ", ";
    os << "]" << std::endl<< std::endl<< std::endl;

    for (std::vector<Relation>::size_type i = 0; i < t.from_relation.size(); i++)
    {
        os << t.from_relation[i] << std::endl;
    }

    return os;
}

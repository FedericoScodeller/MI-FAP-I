#include "network.hpp"

//NETWORK METOHD AND FRIEND FUNCTION
Network::Network(nlohmann::json data)
{
    //GENERAL NETWORK INFORMATION

    bcch_bcch = data["general_information"]["handover_separation"]["bcch->bcch"];
    bcch_tch  = data["general_information"]["handover_separation"]["bcch->tch"];
    tch_bcch  = data["general_information"]["handover_separation"]["tch->bcch"];
    tch_tch   = data["general_information"]["handover_separation"]["tch->tch"];


    min_freq  = data["general_information"]["spectrum"]["min"];
    max_freq  = data["general_information"]["spectrum"]["max"];

    co_site_separetion = data["general_information"]["co_site_separation"];
    co_cell_separetion = data["general_information"]["default_co_cell_separation"];

    if (data["general_information"]["globally_blocked_channels"] != nlohmann::detail::value_t::null)
    {
        for (nlohmann::basic_json<>::size_type i = 0; i < data["general_information"]["globally_blocked_channels"].size(); i++)
        {
            int j = data["general_information"]["globally_blocked_channels"] [i];
            globally_blocked_channels.push_back(j);
        }
    }

    //TRANSMITTER CREATION
    for(nlohmann::basic_json<>::size_type i = 0; i < data["cells"].size(); i++)
    {
        //the first is always a bcch type
        transmitters.push_back(Transmitter(data["cells"][i] , transmitter_type::bcch));
        
        //the others are tch type      
        for(int j = 1; j < data["cells"][i]["demand"] ; j++)
            transmitters.push_back(Transmitter(data["cells"][i] , transmitter_type::tch));
    }
    std::cout << "Tx vector created" << std::endl;
    //CO_CELL & CO_SITE RELATION (questa primo ciclo serve per le relazioni co_sito e co_cella che non sono presenti in "cell relation" del file .json)
    for (std::vector<Transmitter>::size_type i = 0; i < transmitters.size(); i++)
    {
        for (std::vector<Transmitter>::size_type j = 0; j < transmitters.size(); j++)
        {
            if (i!=j)
            {
                if (transmitters[i].Cell()==transmitters[j].Cell())
                {
                    transmitters[i].UpdateRel(Relation(j,co_cell_separetion));
                    transmitters[j].AddToRelation(i);
                }
                else if (transmitters[i].Site()==transmitters[j].Site())
                {
                    transmitters[i].UpdateRel(Relation(j,co_site_separetion));
                    transmitters[j].AddToRelation(i);
                }
            }
        } 
    }
    std::cout << "co_site and co_cell relation created" << std::endl;
    //CELL RELATION FROM JSON
    for (nlohmann::basic_json<>::size_type n = 0; n < data["cell_relations"].size(); n++)
    {
        for (std::vector<Transmitter>::size_type i = 0; i < transmitters.size(); i++)
        {
            for (std::vector<Transmitter>::size_type j = 0; j < transmitters.size(); j++)
            {
                if (transmitters[i].Cell() == data["cell_relations"][n]["to"] && transmitters[j].Cell() == data["cell_relations"][n]["from"])
                {
                    transmitters[i].UpdateRel(Relation(data["cell_relations"][n], j, (*this).HandoverCost(transmitters[j],transmitters[i])));
                    if(!(transmitters[j].InToRelation(i)))
                       transmitters[j].AddToRelation(i); 
                }
            }
        }
        std::cout << n+1<<"/"<< data["cell_relations"].size() << " cell relation done"<< std::endl; 
    }  
    std::cout << "relation from .json created" << std::endl;
      
}


int Network::HandoverCost(Transmitter from, Transmitter to)
{
    /*sono necesserie si informazioni da entrambe le celle che non vengono salvate nel vettore
    relazioni dei trasmettitori, questo perchè in molte funzioni simili servono varie informazioni
    da entrambi i transmettitori, forse usare un puntatore in cell relation potrebbe essere una soluzione migliore?*/
    if (from.Type()==transmitter_type::bcch)
    {
        if (to.Type()==transmitter_type::bcch)
        {
            return (*this).bcch_bcch;
        }
        else
        {
            return (*this).bcch_tch;
        }
    }
    else
    {
        if (to.Type()==transmitter_type::bcch)
        {
            return (*this).tch_bcch;
        }
        else
        {
            return (*this).tch_tch;
        }
    }  
}


void Network::Greedy(void)
{
    /*primo algoritmo di assegnazione della freq, gli algoritmi greedy che ho studiato ho visto che
    non hanno uno specifico criterio per la scelta del ordine su chi servire prima ho quindi usato
    l'ordine in cui sono salvati ma se non è l'unica scelta e ce ne sono di migliori posso cambiarlo*/
    int f_best,f_n,n; 
    //la freq f_n è la freq assegnata ai trasmettitori che causano interferenza  
    double f_best_cost, f_cost;
    for (std::vector<Transmitter>::size_type t = 0; t < (*this).transmitters.size(); t++)
    {
        f_best=-1; //uso -1 come 'flag' per i valori non inizializzalizati

        for (int f=(*this).min_freq; f <= (*this).max_freq; f++)
        {
            f_cost=0.0;

            if (!((std::count((*this).globally_blocked_channels.begin(), (*this).globally_blocked_channels.end(), f))
                && (*this).transmitters[t].BlockedFreq(f)))
            /*entro nel ciclo di valutazione solo se f (la freq correntemente valutata) non è una freq bloccata
            globalmente o localmeente dal tx*/
            {
                for (std::vector<Relation>::size_type r = 0; r < (*this).transmitters[t].FromSize(); r++)
                {
                    n=(*this).transmitters[t].TxFrom(r);
                    f_n=(*this).transmitters[n].Freq();
                    f_cost+=(*this).transmitters[t].CostFreq(n,f_n,f);
                }

                for (std::vector<Relation>::size_type r = 0; r < (*this).transmitters[t].ToSize(); r++)
                {
                    /*questo secondo ciclo è per valutare modifiche di costo ai trasmettitori in cui
                    il tx corrente causa interferenza (infatti gli argomanti sono speculari) questo perchè ho tenuto conto delle assimetrie sia
                    nella presenza che intensita delle interferenze*/
                    n=(*this).transmitters[t].TxTo(r);
                    f_n=(*this).transmitters[n].Freq();
                    f_cost+=(*this).transmitters[n].CostFreq(t,f,f_n);
                }

                if (f_best == -1 || f_best_cost > f_cost)
                {
                    /*aggiorno ho trovato un nuovo minimo o ho un valore valido per inizializzare*/
                    f_best=f;
                    f_best_cost=f_cost;
                }
            }   
        }
        (*this).AssignFreq(t,f_best);
        std::cout << t+1<<"/"<< (*this).transmitters.size() << " freq assigned"<< std::endl;        
    }
    
}


void Network::AssignFreq(int tx, int freq)
{
    /*è usato solo per un controllo d'errore, in effettti poteva stare direttamente in greedy ma
    così può essere riusato e modificato per maggiori controlli d'errore*/
    if (freq==-1)
        std::cout <<"è stato impossibile assegnare una freq al tx: "<< tx << std::endl;
    
    (*this).transmitters[tx].AssignFreq(freq);
    
}


double Network::TotInterference(void)
{
    double cost=0.0;
    int n,f,f_n;
    for (std::vector<Transmitter>::size_type t = 0; t < (*this).transmitters.size(); t++)
    {
        for (std::vector<Relation>::size_type r = 0; r < (*this).transmitters[t].FromSize(); r++)
        {
            /*qui il conto è eseguito solo una volta, calcolo la totale interferenza per una cella
            e poi sommo le totali interferenze in tutto il sistema*/
            n=(*this).transmitters[t].TxFrom(r);
            f=(*this).transmitters[t].Freq();
            f_n=(*this).transmitters[n].Freq();
            cost+=(*this).transmitters[t].CostFreq(n,f_n,f);
        }
    }
    return cost;
}


std::ostream& operator<<(std::ostream& os, const Network& N)
{
    /*non usare se non se si vuole per esteso ogni singolo dettaglio per motivi di debug,
    gia col caso tiny.json è un disatro di informazioni ed ha solo 12 tx, negli altri casi
    dei controlli è praticamente impossible*/

    os << "bcch_bcch: " << N.bcch_bcch << std::endl;
    os << "bcch_tch: "  << N.bcch_tch << std::endl;
    os << "tch_bcch: "  << N.tch_bcch << std::endl;
    os << "tch_tch: "   << N.tch_tch << std::endl;

    os << "range: ["<< N.min_freq << ";" << N.max_freq << "]" << std::endl;

    os << "co_site_separetion:" << N.co_site_separetion << std::endl;
    os << "co_cell_separetion:" << N.co_cell_separetion << std::endl;
    os << std::endl;

    os << "globally_blocked_channels: [";
    for (std::vector<Transmitter>::size_type i = 0; i < N.globally_blocked_channels.size(); i++)
        os << N.globally_blocked_channels[i]<< ", ";
    os << "]" << std::endl << std::endl << std::endl;

    for (std::vector<Transmitter>::size_type i = 0; i < N.transmitters.size(); i++)
    {
        os << "Tx: " << i << std::endl;
        os << N.transmitters[i] << std::endl;
    }
    return os;
}


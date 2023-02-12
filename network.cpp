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

    //inizializza matrici
    std::vector<int> v(transmitters.size(),0);
    std::vector<float> t(transmitters.size(),0.0);
    for (std::vector<Transmitter>::size_type i = 0; i < transmitters.size(); i++)
    {
        separetion.push_back(v);
        same.push_back(t);
        adjacent.push_back(t);
    }
    

    //CO_CELL & CO_SITE RELATION (questa primo ciclo serve per le relazioni co_sito e co_cella che non sono presenti in "cell relation" del file .json)
    for (std::vector<Transmitter>::size_type i = 0; i < transmitters.size(); i++)
    {
        for (std::vector<Transmitter>::size_type j = i+1; j < transmitters.size(); j++)
        {
            if (transmitters[i].Cell()==transmitters[j].Cell())
            {
                separetion[i][j]=separetion[j][i]=co_cell_separetion;
            }
            else if (transmitters[i].Site()==transmitters[j].Site())
            {
                separetion[i][j]=separetion[j][i]=co_site_separetion;
            }
        } 
    }

    std::vector<std::vector<int>> p;
    int tmp_int;
    float tmp_f;
    //CELL RELATION FROM JSON
    for (nlohmann::basic_json<>::size_type n = 0; n < data["cell_relations"].size(); n++)
    {
        p=(*this).FindTx(data["cell_relations"][n]["from"],data["cell_relations"][n]["to"]);

        for (std::vector<int>::size_type i = 0; i < p[0].size(); i++)
        {
            for (std::vector<int>::size_type j = 0; j < p[1].size(); j++)
            {
                //SEPARETION
                if (data["cell_relations"][n]["separetion"] != nlohmann::detail::value_t::null)
                {
                    tmp_int=data["cell_relations"][n]["separetion"];
                    separetion[p[0][i]][p[1][j]]=std::max(tmp_int,separetion[p[0][i]][p[1][j]]);
                }

                if (data["cell_relations"][n]["handover"] == true)
                {
                    separetion[p[0][i]][p[1][j]]=std::max(std::max((*this).HandoverCost(p[0][i],p[1][j]),(*this).HandoverCost(p[1][j],p[0][i]))
                                                    ,separetion[p[0][i]][p[1][j]]);
                }
                separetion[p[1][j]][p[0][i]]=separetion[p[0][i]][p[1][j]];

                //INTERFERENCE
                if (data["cell_relations"][n]["downlink_area_interference"]["same_channel"] != nlohmann::detail::value_t::null)
                {
                    tmp_f=data["cell_relations"][n]["downlink_area_interference"]["same_channel"];
                    same[p[0][i]][p[1][j]] = same[p[0][i]][p[1][j]] + tmp_f;
                    same[p[1][j]][p[0][i]] = same[p[0][i]][p[1][j]];
                }

                if (data["cell_relations"][n]["downlink_area_interference"]["adjacent_channel"] != nlohmann::detail::value_t::null)
                {
                    tmp_f=data["cell_relations"][n]["downlink_area_interference"]["adjacent_channel"];
                    adjacent[p[0][i]][p[1][j]] = adjacent[p[0][i]][p[1][j]] + tmp_f;
                    adjacent[p[1][j]][p[0][i]] = adjacent[p[0][i]][p[1][j]];
                }

            }
        }
    }
}


std::vector<std::vector<int>> Network::FindTx(std::string from,std::string to)
{
    std::vector<std::vector<int>> position(2,std::vector<int>());
    for (std::vector<Transmitter>::size_type i = 0; i < transmitters.size(); i++)
    {
        if (transmitters[i].Cell()==from)
        {
            position[0].push_back(i);
        }
        else if(transmitters[i].Cell()==to)
        {
            position[1].push_back(i);
        }
    }
    return position;
}

int Network::HandoverCost(int from, int to)
{
    if (transmitters[from].Type()==transmitter_type::bcch)
    {
        if (transmitters[to].Type()==transmitter_type::bcch)
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
        if (transmitters[to].Type()==transmitter_type::bcch)
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
    struct FreqCost best_cost, f_cost;
    int f_best;
    for (std::vector<Transmitter>::size_type t = 0; t < (*this).transmitters.size(); t++)
    {
        f_best=-1; //uso -1 come 'flag' per i valori non inizializzalizati
        best_cost.hard_link = 0;
        best_cost.interference = 0.0;

        //assegnazione freq
        for (int f=(*this).min_freq; f <= (*this).max_freq; f++)
        {
            //entro nel ciclo di valutazione solo se f (la freq correntemente valutata) non è una freq bloccata
            //globalmente o localmente 
            if (!((std::count((*this).globally_blocked_channels.begin(), (*this).globally_blocked_channels.end(), f))
                && (*this).transmitters[t].BlockedFreq(f)))
            {
                f_cost=(*this).FreqCost(t,f);

                if (f_best == -1 || best_cost.hard_link > f_cost.hard_link ||(best_cost.hard_link == f_cost.hard_link && best_cost.interference > f_cost.interference))
                {
                    //aggiorno: ho trovato un nuovo minimo o ho un valore valido per inizializzare
                    f_best=f;
                    best_cost=f_cost;
                }
            }   
        }
        (*this).transmitters[t].AssignFreq(f_best);
    }
}

void Network::DSatur(void)
{
    struct FreqCost best_cost, f_cost;
    int f_best,t;
    for (std::vector<Transmitter>::size_type count = 0; count < (*this).transmitters.size(); count++)
    {
        f_best=-1; //uso -1 come 'flag' per i valori non inizializzalizati
        best_cost.hard_link = 0;
        best_cost.interference = 0.0;

        t=(*this).MaxSaturation();
        
        //assegnazione freq
        for (int f=(*this).min_freq; f <= (*this).max_freq; f++)
        {
            //entro nel ciclo di valutazione solo se f (la freq correntemente valutata) non è una freq bloccata
            //globalmente o localmente 
            if (!((std::count((*this).globally_blocked_channels.begin(), (*this).globally_blocked_channels.end(), f))
                && (*this).transmitters[t].BlockedFreq(f)))
            {
                f_cost=(*this).FreqCost(t,f);

                if (f_best == -1 || best_cost.hard_link > f_cost.hard_link ||(best_cost.hard_link == f_cost.hard_link && best_cost.interference > f_cost.interference))
                {
                    //aggiorno: ho trovato un nuovo minimo o ho un valore valido per inizializzare
                    f_best=f;
                    best_cost=f_cost;
                }
            }   
        }
        (*this).transmitters[t].AssignFreq(f_best);
    }
    
}

int Network::MaxSaturation(void)
{
    int tx_max_sat = -1;
    int sat=0, max_sat=0, degree=0, max_degree=0, val_degree=0, max_val_degree=0;
    // sat: saturazione, degree: numero vicini non colorati, val_degree: "valore/intensità" degli hard link
    for (std::vector<Transmitter>::size_type i = 0; i < (*this).transmitters.size(); i++)
    {
        if ((*this).transmitters[i].Freq() == -1)
        {
            sat=0;
            degree=0;
            val_degree=0;

            for (std::vector<Transmitter>::size_type j = 0; j < (*this).transmitters.size(); j++)
            {
                if ((*this).separetion[i][j] > 0)
                {
                    if ((*this).transmitters[j].Freq() != -1)
                    {
                        sat++;
                    }
                    else
                    {
                        degree++;
                        val_degree += (*this).separetion[i][j];
                    }
                }
            }

            if (tx_max_sat == -1 || sat > max_sat || (sat == max_sat && degree > max_degree) || 
                (sat == max_sat && degree == max_degree && val_degree > max_val_degree) )
                //COME DEGREE CONSIDERO IL NUMERO DI VICINI NON COLORATI, LA QUANTITA DI SEPAREAZIONE VOLUTA O COSI COME HO FATTO
            {
                tx_max_sat=i;
                max_sat=sat;
                max_degree=degree;
                max_val_degree=val_degree;
            }
        }
    }
    return tx_max_sat;
}

struct FreqCost Network::FreqCost(int tx, int freq)
{
    struct FreqCost cost;
    for (std::vector<Transmitter>::size_type i = 0; i < (*this).transmitters.size(); i++)
    {
        if ((*this).transmitters[i].Freq() != -1)
        {
            if(separetion[tx][i] > abs(freq - (*this).transmitters[i].Freq()))
            {
                cost.hard_link++;
            }
            else if (abs(freq - (*this).transmitters[i].Freq()) == 0)
            {
                cost.interference+=same[tx][i];
            }
            else if(abs(freq - (*this).transmitters[i].Freq()) == 1)
            {
                cost.interference+=adjacent[tx][i];
            }
          }       
    }
    return cost;
}

//riscrivere per stampare solo i costi di dove ha interferenza e infrazioni
std::ostream& operator<<(std::ostream& os, const Network& N)
{

    struct FreqCost tot_cost;
    

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

    //check broken link
    for (std::vector<Transmitter>::size_type i = 0; i < N.transmitters.size(); i++)
    {
        for (std::vector<Transmitter>::size_type j = i+1; j < N.transmitters.size(); j++)
        {
            if (N.separetion[i][j] > abs( N.transmitters[i].Freq() - N.transmitters[j].Freq()))
            {
                tot_cost.hard_link++;
                os << "Coppia Tx: " << i << " - " << j << " hard link broken" <<std::endl;
            }
            else if (abs( N.transmitters[i].Freq() - N.transmitters[j].Freq())==0
                      && N.same[i][j] > 0.0)
            {
                tot_cost.interference+=N.same[i][j];
                os << "Coppia Tx: " << i << " - " << j << " interference: " << N.same[i][j] <<std::endl;
            }
            else if(abs( N.transmitters[i].Freq() - N.transmitters[j].Freq())==1
                      && N.adjacent[i][j] > 0.0)
            {
                tot_cost.interference+=N.adjacent[i][j];
                os << "Coppia Tx: " << i << " - " << j << " interference: " << N.adjacent[i][j] <<std::endl;
            }
        }
    }

    os << "Total hard links broken: "<< tot_cost.hard_link <<std::endl;
    os << "Total interference: "<< tot_cost.interference <<std::endl;

    

    return os;
}


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

void Network::AssignFreq(int tx)
{
    struct FreqCost best_cost, f_cost;
    int f_best=-1; 
    best_cost.hard_link = 0;
    best_cost.interference = 0.0;

    //assegnazione freq
    for (int f=(*this).min_freq; f <= (*this).max_freq; f++)
    {
        //entro nel ciclo di valutazione solo se f (la freq correntemente valutata) non è una freq bloccata
        //globalmente o localmente 
        if (!((std::count((*this).globally_blocked_channels.begin(), (*this).globally_blocked_channels.end(), f))
            && (*this).transmitters[tx].BlockedFreq(f)))
        {
            f_cost=(*this).FreqCost(tx,f);

            if (f_best == -1 || best_cost.hard_link > f_cost.hard_link ||(best_cost.hard_link == f_cost.hard_link && best_cost.interference > f_cost.interference))
            {
                //aggiorno: ho trovato un nuovo minimo o ho un valore valido per inizializzare
                f_best=f;
                best_cost=f_cost;
            }
        }   
    }
    (*this).transmitters[tx].AssignFreq(f_best);
    return;
}

int Network::Degree(int transmettitore)
{
    int degree=0;

    for (std::vector<Transmitter>::size_type n = 0; n < (*this).transmitters.size(); n++)
    {
        degree+=(*this).separetion[transmettitore][n];
    }

    return degree;
}

int Network::Saturation(int transmettitore)
{
    std::vector<bool> frequenze_occupate((*this).max_freq - (*this).min_freq + 1, false);
    int indice,freq_vicino, separazione;

    //frequenze bloccate globalmente
    for (std::vector<int>::size_type i = 0; i < (*this).globally_blocked_channels.size(); i++)
    {
        indice=(*this).globally_blocked_channels[i]-(*this).min_freq;
        frequenze_occupate[indice]= true;
    }

    //frequenze bloccate localmente dal trasnmettitore
    for (int freq = (*this).min_freq; freq <= (*this).max_freq; freq++)
    {
        indice=freq-(*this).min_freq;
        if ((*this).transmitters[transmettitore].BlockedFreq(freq))
        {
            frequenze_occupate[indice]=true;
        }
    }
    
    //frequenze bloccate dai vicini
    for (std::vector<Transmitter>::size_type vicino = 0; vicino < (*this).transmitters.size(); vicino++)
    {
        freq_vicino = (*this).transmitters[vicino].Freq();
        separazione = (*this).separetion[transmettitore][vicino];
        if (separazione > 0 && freq_vicino != -1)
        {
            for (indice = (freq_vicino - separazione + 1 - (*this).min_freq); indice <= (freq_vicino + separazione - 1 - (*this).min_freq); indice++)
            {
                if (indice >= 0 && indice <= ((*this).max_freq - (*this).min_freq))
                {
                    frequenze_occupate[indice]=true;
                }
                
            }
        }       
    }
    return std::count(frequenze_occupate.begin(), frequenze_occupate.end(), true);
}

int Network::LargestDegree(void)
{
    int degree, largest_degree = 0, transmettitore_largest_degree = -1;
    
    for (std::vector<Transmitter>::size_type transmitter = 0; transmitter < (*this).transmitters.size(); transmitter++)
    {
        if ((*this).transmitters[transmitter].Freq() == -1)
        {
            degree=(*this).Degree(transmitter);

            if (transmettitore_largest_degree == -1 ||  degree > largest_degree )
            {
                transmettitore_largest_degree = transmitter;
                largest_degree = degree;
            }
        }
    }
    return transmettitore_largest_degree;
}

int Network::DSatur(void)
{
    int dsatur, degree, largest_dsatur = 0, largest_degree = 0, transmettitore_largest_dsatur = -1;

    for (std::vector<Transmitter>::size_type transmitter = 0; transmitter < (*this).transmitters.size(); transmitter++)
    {
        if ((*this).transmitters[transmitter].Freq() == -1)
        {
            dsatur=(*this).Saturation(transmitter);
            degree=(*this).Degree(transmitter);

            if (transmettitore_largest_dsatur == -1 || dsatur > largest_dsatur || (dsatur == largest_dsatur && degree > largest_degree))
            {
                transmettitore_largest_dsatur = transmitter;
                largest_dsatur = dsatur;
                largest_degree = degree;
            }
        }
    }
    return transmettitore_largest_dsatur;
}

void Network::MiFap(std::string algoritmo)
{
    int transmitter;
    if (algoritmo == "Greedy")
    {
        for (std::vector<Transmitter>::size_type transmitter = 0; transmitter < (*this).transmitters.size(); transmitter++)
        {
            (*this).AssignFreq(transmitter);
        }
    }
    else if (algoritmo == "LargestDegree")
    {
        transmitter = (*this).LargestDegree();
        while (transmitter != -1)
        {
            (*this).AssignFreq(transmitter);
            transmitter = (*this).LargestDegree();
        }
    }
    else if (algoritmo == "DSatur")
    {
        transmitter = (*this).DSatur();
        while (transmitter != -1)
        {
            (*this).AssignFreq(transmitter);
            transmitter = (*this).DSatur();
        }
    }
    else
    {
        std::cout<<"algoritmo sconoscuto"<<std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    return;
}

std::ostream& operator<<(std::ostream& os, const Network& N)
{

    struct FreqCost tot_cost;

    //check broken link
    for (std::vector<Transmitter>::size_type i = 0; i < N.transmitters.size(); i++)
    {
        for (std::vector<Transmitter>::size_type j = i+1; j < N.transmitters.size(); j++)
        {
            if (N.separetion[i][j] > abs( N.transmitters[i].Freq() - N.transmitters[j].Freq()))
            {
                tot_cost.hard_link++;
            }
            else if (abs( N.transmitters[i].Freq() - N.transmitters[j].Freq())==0
                      && N.same[i][j] > 0.0)
            {
                tot_cost.interference+=N.same[i][j];
            }
            else if(abs( N.transmitters[i].Freq() - N.transmitters[j].Freq())==1
                      && N.adjacent[i][j] > 0.0)
            {
                tot_cost.interference+=N.adjacent[i][j];
            }
        }
    }

    os << tot_cost.hard_link << " " << tot_cost.interference <<std::endl;

    

    return os;
}


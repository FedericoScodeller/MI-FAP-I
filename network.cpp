// file network.cpp

#include "network.hpp"

CellRelation::CellRelation(nlohmann::json relation){
   cell_bonded=relation["to"];
   
   if(relation["handover"]==nlohmann::detail::value_t::null)
      handover=false;
   else
      handover=true;
   
   if(relation["downlink_area_interference"]["same_channel"] == nlohmann::detail::value_t::null)
      same_channel=0.0;
   else
      same_channel=relation["downlink_area_interference"]["same_channel"];

   if(relation["downlink_area_interference"]["adjacent_channel"] == nlohmann::detail::value_t::null)
      adjacent_channel=0.0;
   else
      adjacent_channel=relation["downlink_area_interference"]["adjacent_channel"];
}

std::ostream& operator<<(std::ostream& os, const CellRelation& r){
   os << "cell bond: " << r.cell_bonded << std::endl;
   if (r.handover)
   {
      os << "handover: true "<< std::endl;
   }
   else
   {
      os << "handover: false "<< std::endl;
   }
   os << "same_channel: " << r.same_channel << std::endl;
   os << "adjacent_channel: " << r.adjacent_channel << std::endl;
   return os;
}

Transmitter::Transmitter(nlohmann::json cell, transmitter_type channel,int n_freq, int min_freq){

   assigned_freq=-1;
   id_cell = cell["id"];
   site = cell["site"];
   type = channel;
   freq_cost.resize(n_freq,0.0);
   for (int i=0; i < cell["blocked_channels"].size();i++)
   {
      freq_cost[cell["blocked_channels"][i]-min_freq]= -1.0;
   }
   
}

//ho esagerato su questo costruttore? avrei dovuto creare più funzioni o creare un semplice costruttore e poi aggiungere i dati con altri metodi?
Network::Network(nlohmann::json data){
      
      bcch_bcch = data["general_information"]["handover"]["bcch->bcch"];
      bcch_tch  = data["general_information"]["handover"]["bcch->tch"];
      tch_bcch  = data["general_information"]["handover"]["tch->bcch"];
      tch_tch   = data["general_information"]["handover"]["tch->tch"];

      
      min_freq  = data["general_information"]["spectrum"]["min"];
      max_freq  = data["general_information"]["spectrum"]["max"];

      co_site_separetion = data["general_information"]["co_site_separation"];
      co_cell_separetion = data["general_information"]["default_co_cell_separation"];

      //creazioni del vettore trasmettitori
      
      for(auto& cell:data["cells"]){
         transmitters.push_back(Transmitter(cell , transmitter_type::bcch , max_freq - min_freq + 1, min_freq));
         for(i = 0; i < cell["demand"] - 1; i++){
            transmitters.push_back(Transmitter(cell , transmitter_type::tch , max_freq - min_freq + 1));
         }
      }
      
      //creazione e aggiornamento vincoli per trasmettittori

      for(auto& reletion::data["cell_relations"]){
         for(int i=0; i < transmitters.size; i++){
            if(id_cell == relation["from"]){
               transmitters[i].relations.push_back(CellRelation(relation))
            }
         }
      }
}
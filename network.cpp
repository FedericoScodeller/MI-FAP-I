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

Transmitter::Transmitter(nlohmann::json cell, transmitter_type channel,int n_freq, int offset){
   
   assigned_freq=-1;
   id_cell = cell["id"];
   site = cell["site"];
   type = channel;
   freq_cost.resize(n_freq,0.0);
   
   if (cell["blocked_channels"] != nlohmann::detail::value_t::null)
   {
      for (int i=0; i < cell["blocked_channels"].size();i++)
      {
         int j = cell["blocked_channels"][i]; //library create some problem without this 
         freq_cost[j - offset]= -1.0;
      }  
   }
}

std::ostream& operator<<(std::ostream& os, const Transmitter& t){
   os << "freq: " << t.assigned_freq << std::endl;
   os << "id: " << t.id_cell << std::endl;
   os << "site: " << t.site << std::endl;

   if (t.type == transmitter_type::bcch)
      os << "type: bcch" << std::endl;
   else
      os << "type: tch" << std::endl;
   
   os << "freq cost: [ ";
   for (int i = 0; i < t.freq_cost.size(); i++)
      os <<t.freq_cost[i] << " ";
   os << "]" << std::endl;
   for (int i = 0; i < t.relations.size(); i++)
   {
      os << t.relations[i] << std::endl;
   }
   os << std::endl;
   return os;
}

Network::Network(nlohmann::json data){
      
      bcch_bcch = data["general_information"]["handover_separation"]["bcch->bcch"];
      bcch_tch  = data["general_information"]["handover_separation"]["bcch->tch"];
      tch_bcch  = data["general_information"]["handover_separation"]["tch->bcch"];
      tch_tch   = data["general_information"]["handover_separation"]["tch->tch"];

      
      min_freq  = data["general_information"]["spectrum"]["min"];
      max_freq  = data["general_information"]["spectrum"]["max"];

      co_site_separetion = data["general_information"]["co_site_separation"];
      co_cell_separetion = data["general_information"]["default_co_cell_separation"];
}

void Network::AddTransmitters(nlohmann::json cells){

    for(int i = 0; i < cells.size(); i++){
         //the first is always a bcch type
         transmitters.push_back(Transmitter(cells[i] , transmitter_type::bcch , max_freq - min_freq + 1, min_freq));
         //the others are tch type         
         for(int j = 1; j < cells[i]["demand"] ; j++){
            transmitters.push_back(Transmitter(cells[i] , transmitter_type::tch , max_freq - min_freq + 1, min_freq));
         }
      }

}

void Transmitter::AddRelation(nlohmann::json rel){
   relations.push_back(CellRelation(rel));
}

void Network::AddRelations(nlohmann::json cell_rels){

   for(int n = 0 ; n < cell_rels.size(); n++)
      for(int i=0; i < transmitters.size(); i++)
         if(transmitters[i].IdCell() == cell_rels[n]["from"])
            transmitters[i].AddRelation(cell_rels[n]);

}

std::ostream& operator<<(std::ostream& os, const Network& N){
   os << "bcch_bcch: " << N.bcch_bcch << std::endl;
   os << "bcch_tch: "  << N.bcch_tch << std::endl;
   os << "tch_bcch: "  << N.tch_bcch << std::endl;
   os << "tch_tch: "   << N.tch_tch << std::endl;
      
   os << "range: ["<< N.min_freq << ";" << N.max_freq << "]" << std::endl;

   os << "co_site_separetion:" << N.co_site_separetion << std::endl;
   os << "co_cell_separetion:" << N.co_cell_separetion << std::endl;
   os << std::endl;

   for (int i = 0; i < N.transmitters.size(); i++)
   {
      os << N.transmitters[i] << std::endl;
   }
   return os;
}
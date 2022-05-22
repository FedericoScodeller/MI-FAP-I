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

/*void Network::AssignFreq_mk1(void)
{
      for (int n = 0; n < transmitters.size(); n++)
      {
         int freq_crit = -1; //funziona come un flag
         int degree_freq_crit;
         int degree_freq_i;
         int freq_assigned;
         
         //ricerca critico
         for (int i = 0; i < transmitters.size(); i++)
         {
            if (transmitters[i].AssignedFreq() == -1)
            {
               degree_freq_i=transmitters[i].Degree();
               
               if (freq_crit == -1 || degree_freq_i > degree_freq_crit)
               {
                  freq_crit=i;
                  degree_freq_crit=degree_freq_i;
               }
            }
         }
         //freq assign
         freq_assigned=transmitters[freq_crit].AssignFreq();
         
         //aggiorna freq
         for (int i = 0; i < transmitters.size(); i++)
         {
            transmitters[i].UpdateFreqCost(freq_assigned, -1.0);
         }
      
      }
      
}*/

int Network::LargestDegree(void)
{
   int crit_tran = -1; //il -1 funziona come un flag
   int degree_crit_tran;
   int i_degree;
   
   
   //ricerca critico
   for (int i = 0; i < transmitters.size(); i++)
   {
      if (transmitters[i].AssignedFreq() == -1) //eseguo il controllo solo su transmettitori che non hanno una freq
      {
         i_degree=transmitters[i].Degree();
         
         if (crit_tran == -1 || i_degree > degree_crit_tran) //se non ho inizializzato il transmettitore critico o trovo uno con degree maggiore
         {
            crit_tran=i;
            degree_crit_tran=i_degree;
         }
      }
   }

   //mi serve un errore nel caso non riesca a trovare nessun trasmettitore critico o posso solo restituire -1 e mettere controlli dopo la funzione

   return crit_tran;
}

int Transmitter::Degree(void)
{
   int degree = 0;
   for (int i = 0; i < freq_cost.size(); i++)
   {
      if (freq_cost[i] < 0.0)
      {
         degree++;
      }
   }
   return degree;
}

void Transmitter::AssignFreq(void)
{
   //rimane -1 in caso di fallimento
   //non servono return poichè è un valore sempre disponibile
   for (int i = 0; i < freq_cost.size(); i++)
   {
      if (freq_cost[i] >= 0.0 && (assigned_freq == -1 || (assigned_freq != -1 && freq_cost[i] < freq_cost [assigned_freq])))
      {   
         assigned_freq=i;
      }
   }
   
   if (assigned_freq == -1)//entro in questo loop solo se non ho positivi
   {
      for (int i = 0; i < freq_cost.size(); i++)
      {
         if (assigned_freq == -1 || freq_cost[i] > freq_cost [assigned_freq])
         {
            assigned_freq=i;
         }
      }
   }
}


void Network::AssignFreq_mk2(void) //devo mettere dei controlli non ho protezioni da errori
{
   int crit_transmitter, assigned_freq;
   for (int i = 0; i < transmitters.size(); i++)
   {
      crit_transmitter = (*this).LargestDegree();
      transmitters[crit_transmitter].AssignFreq();
      (*this).UpdateCost(crit_transmitter,transmitters[crit_transmitter].AssignedFreq());
   }
}

void Network::UpdateCost(int changed_transmitter, int changed_freq)
{
   int r;
   int handover_cost;

   for (int i = 0; i < transmitters.size(); i++)
   {
      r = transmitters[changed_transmitter].FindRelation(transmitters[i].IdCell());
      if (i != changed_transmitter)
      {
      
         //co_cell imply co_site and co_cell has a stronger separetion anyways
         if (transmitters[changed_transmitter].IdCell()==transmitters[i].IdCell())
         {
            //This for cycle is used many times, do I made it in to a function?
            for (int j = changed_freq - co_cell_separetion; j <= changed_freq + co_cell_separetion ; j++)
            {
               if (j >= 0 && j < (*this).NFreqAvailable()) //to avoid segmentation fault
               {
                  transmitters[i].UpdateFreqCost(j,-1.0);
               }
               
            }
            
         }
         else if (transmitters[changed_transmitter].Site()==transmitters[i].Site())
         {
            for (int j = changed_freq - co_site_separetion; j <= changed_freq + co_site_separetion ; j++)
            {
               if (j >= 0 && j < (*this).NFreqAvailable())
               {
                  transmitters[i].UpdateFreqCost(j,-1.0);
               }
            }
         }
         else if(r != -1)
         {
            
            if(transmitters[changed_transmitter].Handover(r))
            {
               //selezionare il tipo di relazione
               if (transmitters[changed_transmitter].Type() == transmitter_type::bcch && transmitters[i].Type() == transmitter_type::bcch)
               {
                  handover_cost=bcch_bcch;
               }
               else if (transmitters[changed_transmitter].Type() == transmitter_type::bcch && transmitters[i].Type() == transmitter_type::tch)
               {
                  handover_cost=bcch_tch;
               }
               else if (transmitters[changed_transmitter].Type() == transmitter_type::tch && transmitters[i].Type() == transmitter_type::bcch)
               {
                  handover_cost=tch_bcch;
               }
               else if (transmitters[changed_transmitter].Type() == transmitter_type::tch && transmitters[i].Type() == transmitter_type::tch)
               {
                  handover_cost=tch_tch;
               }
               else
               {
                  std::cout << "Error handover cost" << std::endl;
               }
               
               //usare il ciclo per update cost
               for (int j = changed_freq - handover_cost; j <= changed_freq + handover_cost ; j++)
               {
                  if (j >= 0 && j < (*this).NFreqAvailable())
                  {
                     transmitters[i].UpdateFreqCost(j,-1.0);
                  }
               }

            }
            /*else 
            {
               if (transmitters[changed_transmitter].SameChannel(r) != 0.0)
               {
                  transmitters[i].UpdateFreqCost(changed_freq,transmitters[changed_transmitter].SameChannel(r));
               }

               if (transmitters[changed_transmitter].AdjacentChannel(r) != 0.0 && (changed_freq - 1 >= 0) )
               {
                  transmitters[i].UpdateFreqCost(changed_freq - 1,transmitters[changed_transmitter].AdjacentChannel(r));
               }

               if (transmitters[changed_transmitter].AdjacentChannel(r) != 0.0 && (changed_freq + 1 < (*this).NFreqAvailable()) )
               {
                  transmitters[i].UpdateFreqCost(changed_freq + 1,transmitters[changed_transmitter].AdjacentChannel(r));
               }
            }*/
         }
      }
   }
}
int Transmitter::FindRelation(std::string id)
{
   for (int i = 0; i < relations.size(); i++)
   {
      if (id == relations[i].CellId())
      {
         return i;
      }
   }
   return -1;
}

void Transmitter::UpdateFreqCost(int freq, double cost)
{
   if (freq_cost[freq] < 0 || cost < 0)
   {
      freq_cost[freq] = -abs(freq_cost[freq])-abs(cost);
   }
   else
   {
      freq_cost[freq] += abs(cost);
   }
}



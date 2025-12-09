#include "../include/Input.hh"
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>

Input::Input(const json& data)
{

   const json& info=data["general_information"];
   //SPECTRUM

   const int freq_min = info["spectrum"]["min"];
   const int freq_max = info["spectrum"]["max"];
   tot_ch = freq_max - freq_min + 1;

   std::vector<bool> glob_blk_ch(tot_ch,false);
   if(info.contains("globally_blocked_channels"))
   {
      const json& blk_ch = info["globally_blocked_channels"];
      const size_t blk_ch_size = blk_ch.size();
      for(size_t i = 0;i < blk_ch_size;i++)
      {
         glob_blk_ch[ blk_ch[i].get<unsigned>() - freq_min ] = true;
      }
   }

   //TX DATA
   std::vector<std::string> tx_site, tx_cell;
   std::unordered_map<std::string, unsigned> map_cell;

   enum TxType {bcch, tch};
   std::vector<TxType> tx_type;

   const json& cells = data["cells"];
   const size_t cell_number =cells.size();

   std::vector<bool> tx_blk_ch;
   size_t demand;

   for(size_t i = 0; i < cell_number; i++)
   {
      tx_blk_ch = glob_blk_ch;
      if(cells[i].contains("blocked_channels"))
      {
         const json& loc_blk_ch = cells[i]["blocked_channels"];
         const size_t loc_blk_ch_size = loc_blk_ch.size();
         for(size_t j = 0;j < loc_blk_ch_size;j++)
         {
            tx_blk_ch[ loc_blk_ch[j].get<unsigned>() - freq_min ] = true;
         }
      }

      map_cell [cells[i]["id"].get<std::string>()] = tx_cell.size();

      demand = cells[i]["demand"];
      for(size_t j = 0; j < demand; j++)
      {
         tx_cell.push_back(cells[i]["id"]);
         tx_site.push_back(cells[i]["site"]);
         tx_type.push_back(j == 0 ? bcch : tch);
         mat_blk_ch.push_back( tx_blk_ch);
      }
   }

   //TX RELATION

   network_size = tx_cell.size();

   ch_sep.resize(network_size, std::vector<int>(network_size, 0));
   same_ch_int.resize(network_size, std::vector<float>(network_size, 0.0));
   adj_ch_int.resize(network_size, std::vector<float>(network_size, 0.0));

   const unsigned co_site_sep = info["co_site_separation"];
   const unsigned co_cell_sep = info["default_co_cell_separation"];

   for (size_t i = 0; i < network_size; i++)
   {
      for (size_t j = i + 1; j < network_size; j++)
      {
         if (tx_cell[i] == tx_cell[j])
            ch_sep[i][j] = ch_sep[j][i] = co_cell_sep;
         else if (tx_site[i] == tx_site[j])
            ch_sep[i][j] = ch_sep[j][i] = co_site_sep;
      }
   }
   //CELL RELATION
   const std::vector<std::vector<int>> handover = { {info["handover_separation"]["bcch->bcch"],info["handover_separation"]["bcch->tch"]},
                                                 {info["handover_separation"]["tch->bcch"], info["handover_separation"]["tch->tch"]}};

   unsigned idx_from,idx_to;
   std::string from_cell_id, to_cell_id;
   const json& cell_rel = data["cell_relations"];
   const size_t cell_rel_size = cell_rel.size();

   for(size_t n = 0; n < cell_rel_size; n++)
   {
      idx_from = idx_to = 0;
      from_cell_id = cell_rel[n]["from"];
      to_cell_id = cell_rel[n]["to"];

      idx_from = map_cell[from_cell_id];
      idx_to = map_cell[to_cell_id];

      for (size_t i = idx_from; i < network_size && tx_cell[i] == from_cell_id; i++)
      {
         for (size_t j = idx_to; i < network_size &&  tx_cell[j] == to_cell_id; j++)
         {
            if (cell_rel[n].contains("separation"))
               ch_sep[i][j] = std::max(ch_sep[i][j],cell_rel[n]["separation"].get<int>());
            if (cell_rel[n].contains("handover") && cell_rel[n]["handover"]) //devo trovare un modo migliore per questa condizione
               ch_sep[i][j] = std::max(ch_sep[i][j],handover[tx_type[i]][tx_type[j]]);
            if (cell_rel[n].contains("downlink_area_interference") && cell_rel[n]["downlink_area_interference"].contains("same_channel"))
               same_ch_int[i][j] = data["cell_relations"][n]["downlink_area_interference"]["same_channel"];
            if (cell_rel[n].contains("downlink_area_interference") && cell_rel[n]["downlink_area_interference"].contains("adjacent_channel"))
               adj_ch_int[i][j] =data["cell_relations"][n]["downlink_area_interference"]["adjacent_channel"];
         }
      }
   }

   //ADJ MATRIX
   adj_mat_from.resize(network_size);
   adj_mat_to.resize(network_size);
   degree.resize(network_size);

   for(size_t i=0; i < network_size; i++)
      for (size_t j = 0; j < network_size; j++)
         if(ch_sep[i][j] != 0 || same_ch_int[i][j] != 0 || adj_ch_int[i][j] != 0 )
         {
            adj_mat_from[i].push_back(j);
            adj_mat_to[j].push_back(i);
            degree[j] += ch_sep[i][j];
         }
}

std::ostream &operator<<(std::ostream &os, const Input &in)
{

   os << "#TX " << in.network_size << " #CH " << in.tot_ch;


#ifdef DEBUG

   os << std::endl << "CHANNELS BLOCKED" << std::endl<< std::endl;
   for (size_t tx = 0; tx < in.network_size; tx++)
   {
      for (size_t f = 0; f < in.tot_ch; f++)
         os << in.mat_blk_ch[tx][f] << " ";
      os << std::endl;
   }

   os << std::endl;

   os << "SEPARETION" << std::endl << std::endl;
   for (size_t i = 0; i < in.network_size; i++)
   {
      for (size_t j = 0; j < in.network_size; j++)
         os << in.ch_sep[i][j] << " ";
      os << std::endl;
   }

   os << std::endl;

   os << std::fixed << std::setprecision(2);

   os << "INTERFERENCE COST SAME CHANNEL" << std::endl << std::endl ;
   for (size_t i = 0; i < in.network_size; i++)
   {
      for (size_t j = 0; j < in.network_size; j++)
         os << in.same_ch_int[i][j] << " ";
      os << std::endl;
   }

   os << std::endl;

   os << "INTERFERENCE COST ADJ CHANNEL" << std::endl << std::endl;
   for (size_t i = 0; i < in.network_size; i++)
   {
      for (size_t j = 0; j < in.network_size; j++)
         os << in.adj_ch_int[i][j] << " ";
      os << std::endl;
   }

   os << std::endl;

   os << "ADJ MATRIX FROM" << std::endl << std::endl;
   for (size_t i = 0; i < in.network_size; i++)
   {
      for (size_t j = 0; j < in.adj_mat_from[i].size(); j++)
         os << in.adj_mat_from[i][j] << " ";
      os << std::endl;
   }

   os << std::endl;

   os << "ADJ MATRIX TO" << std::endl << std::endl;
   for (size_t i = 0; i < in.network_size; i++)
   {
      for (size_t j = 0; j < in.adj_mat_to[i].size(); j++)
         os << in.adj_mat_to[i][j] << " ";
      os << std::endl;
   }

   os << std::endl;

   os << "DEGREE" << std::endl <<std::endl;
   for (size_t tx = 0; tx < in.network_size; tx++)
   {

         os << tx << " : " << in.Degree(tx) <<std::endl;

   }
#endif
   return os;
}

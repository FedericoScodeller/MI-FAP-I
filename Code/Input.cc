#include "Input.hh"
#include <vector>

Input::Input(nlohmann::json data)
{
   std::vector<bool> glob_blocked_ch, tmp_ch_vect;

   id=data["general_information"]["scenario_id"];
   freq_min = data["general_information"]["spectrum"]["min"];
   freq_max = data["general_information"]["spectrum"]["max"];
   tot_ch = freq_max - freq_min + 1;

   glob_blocked_ch.resize(tot_ch,false);

   for (nlohmann::basic_json<>::size_type i = 0; i < data["general_information"]["globally_blocked_channels"].size();i++)
   {
      glob_blocked_freq.push_back(static_cast<unsigned>(data["general_information"]["globally_blocked_channels"][i]));
      glob_blocked_ch[ChFreq(glob_blocked_freq[i])] = true;
   }

   co_site_sep = data["general_information"]["co_site_separation"];
   co_cell_sep = data["general_information"]["default_co_cell_separation"];

   bcch_bcch = data["general_information"]["handover_separation"]["bcch->bcch"];
   bcch_tch = data["general_information"]["handover_separation"]["bcch->tch"];
   tch_bcch = data["general_information"]["handover_separation"]["tch->bcch"];
   tch_tch = data["general_information"]["handover_separation"]["tch->tch"];

   //std::vector<unsigned> local_block_freq;//non necessario non da alcun vantaggio o svantaggio nel tempo

   for (nlohmann::basic_json<>::size_type i = 0; i < data["cells"].size(); i++)
   {
      tmp_ch_vect = glob_blocked_ch;

      // for (nlohmann::basic_json<>::size_type j = 0; j < data["cells"][i]["blocked_channels"].size();j++)
      //     tmp_ch_vect[ChFreq(static_cast<unsigned>(data["cells"][i]["blocked_channels"][j]))] = true;


      network.push_back(Transmitter(data["cells"][i], TxType::bcch));

      //altenativa canali bloccati
      //local_block_freq=network.back().BlockedFreq();
      // for(auto freq:local_block_freq)
      //    tmp_ch_vect[ChFreq(freq)] = true;
      for(auto freq:network.back().BlockedFreq())
         tmp_ch_vect[ChFreq(freq)] = true;



      tx_blocked_ch.push_back(tmp_ch_vect);

      for (nlohmann::basic_json<>::size_type j = 1;j < data["cells"][i]["demand"]; j++)
      {
         network.push_back(Transmitter(data["cells"][i], TxType::tch));
         tx_blocked_ch.push_back(tmp_ch_vect);
      }
   }
   network_size = network.size();
   ch_sep.resize(network_size, std::vector<unsigned>(network_size, 0));
   same_ch_int.resize(network_size, std::vector<float>(network_size, 0.0));
   adj_ch_int.resize(network_size, std::vector<float>(network_size, 0.0));

   for (std::vector<Transmitter>::size_type i = 0; i < network_size; i++)
   {
      for (std::vector<Transmitter>::size_type j = i + 1; j < network_size; j++)
      {
         if (network[i].Cell() == network[j].Cell())
            ch_sep[i][j] = ch_sep[j][i] =co_cell_sep; // maybe the max is pointless cell sep > site sep almeno nei nostri casi
         else if (network[i].Site() == network[j].Site())
            ch_sep[i][j] = ch_sep[j][i] = co_site_sep;
      }
   }


   // special separetion and interf
   std::vector<unsigned> from, to;
   for (nlohmann::basic_json<>::size_type n = 0; n < data["cell_relations"].size(); n++)
   {
      // from = FindTxFromCell(data["cell_relations"][n]["from"]);
      // to = FindTxFromCell(data["cell_relations"][n]["to"]);
      // the improvement is at worst 30% if there is a considerable size
      from = FindTxFromCellOpt(data["cell_relations"][n]["from"]);
      to = FindTxFromCellOpt(data["cell_relations"][n]["to"]);
      for (auto i : from)
      {
         for (auto j : to)
         {
            if (!data["cell_relations"][n]["separation"].is_null())
               ch_sep[i][j] = ch_sep[j][i] = std::max(ch_sep[i][j],static_cast<unsigned>(data["cell_relations"][n]["separation"]));
            if (data["cell_relations"][n]["handover"]==true) // maybe the == true is unecessary? no super necessary!
               ch_sep[i][j] = ch_sep[j][i] = std::max(ch_sep[i][j], std::max(Handover(i, j), Handover(j, i)));
            if (!data["cell_relations"][n]["downlink_area_interference"]["same_channel"].is_null())
               same_ch_int[i][j] = same_ch_int[j][i] += static_cast<float>(data["cell_relations"][n]["downlink_area_interference"]["same_channel"]);
            if (!data["cell_relations"][n]["downlink_area_interference"]["adjacent_channel"].is_null())
               adj_ch_int[i][j] = adj_ch_int[j][i] += static_cast<float>(data["cell_relations"][n]["downlink_area_interference"]["adjacent_channel"]);
         }
      }
   }
}

std::vector<unsigned> Input::FindTxFromCell(std::string cell) const
{
   std::vector<unsigned> out;
   for (unsigned i = 0; i < network_size; i++)
      if (cell == network[i].Cell())
         out.push_back(i);
   return out; //thanks to the RVO from C++11 onward it's O(1) and really easy to use I don't even return by reference (const) to be fast!
}

std::vector<unsigned> Input::FindTxFromCellOpt(std::string cell) const
{
   std::vector<unsigned> out;
   unsigned i = 0;
   while (cell != network[i].Cell())
      i++;
   while (i < network_size && cell == network[i].Cell())
      out.push_back(i++);
   return out;
}

unsigned Input::Handover(unsigned from, unsigned to) const
{
   if (network[from].Type() == TxType::bcch)
      if (network[to].Type() == TxType::bcch)
         return bcch_bcch;
      else
         return bcch_tch;
   else if (network[to].Type() == TxType::bcch)
      return tch_bcch;
   else
      return tch_tch;
}

std::ostream &operator<<(std::ostream &os, const Input &in)
{
   os << "GENERAL INFO" << std::endl<< std::endl;

   os << "id: " << in.id << std::endl;
   os << "network size: " << in.network_size << std::endl;
   os << "number of channels: " << in.tot_ch << std::endl;
#ifdef DEBUG
   os << "freq min: " << in.freq_min << std::endl;
   os << "freq max: " << in.freq_max << std::endl;

   os << "globally blocked freq: [ ";
   for (auto &x : in.glob_blocked_freq)
   {
      os << x;
      if (&x != &in.glob_blocked_freq.back())
         os << ", ";
   }
   os << " ]" << std::endl<< std::endl;

   os << "co site separetion: " << in.co_site_sep << std::endl;
   os << "co cell separetion: " << in.co_cell_sep << std::endl<< std::endl;

   os << "bcch->bcch: " << in.bcch_bcch << std::endl;
   os << "bcch->tch: " << in.bcch_tch << std::endl;
   os << "tch->bcch: " << in.tch_bcch << std::endl;
   os << "tch->tch: " << in.tch_tch << std::endl<< std::endl;


   os << std::endl << "NETWORK" << std::endl<< std::endl;
   for (size_t tx=0; tx < in.network_size; tx++)
   {
      os << "tx: " << tx << std::endl;
      os << in.network[tx] << std::endl;
   }

   os << std::endl;

   os << std::endl << "CHANNELS BLOCKED" << std::endl<< std::endl;
   for (size_t tx = 0; tx < in.network_size; tx++)
   {
      for (size_t f = 0; f < in.tot_ch; f++)
         os << in.tx_blocked_ch[tx][f] << " ";
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
#endif
   return os;
}

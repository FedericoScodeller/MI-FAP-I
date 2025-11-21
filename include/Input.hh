#ifndef INPUT_HH
#define INPUT_HH
#include "Transmitter.hh"
#include <string>

class Input
{
   protected:
   // note frequence(freq) and channel(ch) are not the same thing but are related
   // as follow freq are used in the json file and in the output print, ch are
   // used in the code due to the alignement with array index (this should go in
   // the doc)
   std::string id;
   unsigned freq_min;
   unsigned freq_max;
   unsigned tot_ch;
   unsigned network_size;
   std::vector<unsigned> glob_blocked_freq;

   unsigned co_site_sep;
   unsigned co_cell_sep;
   unsigned bcch_bcch;
   unsigned bcch_tch;
   unsigned tch_bcch;
   unsigned tch_tch;

   std::vector<std::vector<unsigned>> ch_sep;
   std::vector<std::vector<float>> same_ch_int;
   std::vector<std::vector<float>> adj_ch_int;
   std::vector<std::vector<bool>> tx_blocked_ch;

   std::vector<Transmitter> network;


   // need better names
   unsigned FreqCh(unsigned ch) { return ch + freq_min; };
   unsigned ChFreq(unsigned freq) { return freq - freq_min; };

   std::vector<unsigned> FindTxFromCell(std::string cell) const; //sfrutta RVO dal C++11 quindi è efficente O(1)
   std::vector<unsigned> FindTxFromCellOpt(std::string cell) const;
   unsigned Handover(unsigned from, unsigned to) const;

   public:
   Input(nlohmann::json data);

   unsigned NetworkSize (void) const {return network_size;};
   unsigned TotCh (void) const {return tot_ch;};
   unsigned ChSep(unsigned tx_from, unsigned tx_to) const {return ch_sep[tx_from][tx_to];};
   float SameChInt(unsigned tx_from, unsigned tx_to) const {return same_ch_int[tx_from][tx_to];};
   float AdjChInt(unsigned tx_from, unsigned tx_to) const {return adj_ch_int[tx_from][tx_to];};

   friend std::ostream &operator<<(std::ostream &os, const Input &in);
};
#endif

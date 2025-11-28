#ifndef INPUT_HH
#define INPUT_HH
#include "../libs/json.hh"
#include <ostream>
#include <vector>

using nlohmann::json;

class Input
{

   friend std::ostream &operator<<(std::ostream &os, const Input &in);

   public:

   Input(const json& data);

   unsigned NetworkSize (void) const {return network_size;};
   unsigned TotCh (void) const {return tot_ch;};


   unsigned ChSep(unsigned tx_from, unsigned tx_to) const {return ch_sep[tx_from][tx_to];};
   float SameChInt(unsigned tx_from, unsigned tx_to) const {return same_ch_int[tx_from][tx_to];};
   float AdjChInt(unsigned tx_from, unsigned tx_to) const {return adj_ch_int[tx_from][tx_to];};
   bool ChBlocked (unsigned tx, unsigned ch) const {return mat_blk_ch[tx][ch];};

   std::vector<unsigned> AdjTx(unsigned tx) const {return adj_mat[tx];};

   protected:

   unsigned tot_ch;
   unsigned network_size;

   std::vector<std::vector<unsigned>> ch_sep;
   std::vector<std::vector<float>> same_ch_int;
   std::vector<std::vector<float>> adj_ch_int;

   std::vector<std::vector<bool>> mat_blk_ch;

   std::vector<std::vector<unsigned>> adj_mat;

   unsigned Separetion(unsigned from, unsigned to) const;


};
#endif

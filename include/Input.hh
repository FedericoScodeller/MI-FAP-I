#ifndef INPUT_HH
#define INPUT_HH
#include "../libs/json.hh"
#include <ostream>
#include <vector>

using nlohmann::json;
//uso RVO, implementato da compilatori moderni e versioni più recenti dello std c++ (e per recenti intendo 2011), se non disponibile va cambiate alcune cose nei return dei vector
class Input
{

   friend std::ostream &operator<<(std::ostream &os, const Input &in);

   public:

      Input(const json& data);

      unsigned NetworkSize (void) const {return network_size;};
      int TotCh (void) const {return tot_ch;};

      int ChSep(unsigned tx_from, unsigned tx_to) const {return ch_sep[tx_from][tx_to];};
      int SameChInt(unsigned tx_from, unsigned tx_to) const {return same_ch_int[tx_from][tx_to];};
      int AdjChInt(unsigned tx_from, unsigned tx_to) const {return adj_ch_int[tx_from][tx_to];};
      bool ChBlocked (unsigned tx, int ch) const {return mat_blk_ch[tx][ch];};

      std::vector<unsigned> AdjTxFrom(unsigned tx) const {return adj_mat_from[tx];};
      std::vector<unsigned> AdjTxTo(unsigned tx) const {return adj_mat_to[tx];};

      std::vector<std::vector<bool>> MatBlkCh(void) const {return mat_blk_ch;};

      int Degree(unsigned tx) const {return degree[tx];};

   protected:

      unsigned tot_ch;
      unsigned network_size;

      float min_interf;

      std::vector<std::vector<int>> ch_sep;
      std::vector<std::vector<int>> same_ch_int;
      std::vector<std::vector<int>> adj_ch_int;

      std::vector<std::vector<bool>> mat_blk_ch;

      std::vector<std::vector<unsigned>> adj_mat_from;
      std::vector<std::vector<unsigned>> adj_mat_to;
      std::vector<int> degree;


      unsigned Separetion(unsigned from, unsigned to) const;




};
#endif

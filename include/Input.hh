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
   int TotCh (void) const {return tot_ch;};


   int ChSep(unsigned tx_from, unsigned tx_to) const {return ch_sep[tx_from][tx_to];};
   float SameChInt(unsigned tx_from, unsigned tx_to) const {return same_ch_int[tx_from][tx_to];};
   float AdjChInt(unsigned tx_from, unsigned tx_to) const {return adj_ch_int[tx_from][tx_to];};
   bool ChBlocked (unsigned tx, int ch) const {return mat_blk_ch[tx][ch];};

   std::vector<unsigned> AdjTxFrom(unsigned tx) const {return adj_mat_from[tx];};
   std::vector<unsigned> AdjTxTo(unsigned tx) const {return adj_mat_to[tx];};

   int Degree(unsigned tx) const {return degree[tx];};

   protected:

   unsigned tot_ch;
   unsigned network_size;

   std::vector<std::vector<int>> ch_sep;
   std::vector<std::vector<float>> same_ch_int;
   std::vector<std::vector<float>> adj_ch_int;

   std::vector<std::vector<bool>> mat_blk_ch;

   std::vector<std::vector<unsigned>> adj_mat_from;
   std::vector<std::vector<unsigned>> adj_mat_to; //Potrebbe essere inutile e occupa memoria e tempo per nulla, potresti dediare una funzione al calcolo solo quando serve
   //ad esempio Degree(tx), calcola il degree per tale tx tanto ti serve solo in alcuni algoritmi
   std::vector<int> degree;


   unsigned Separetion(unsigned from, unsigned to) const;




};
#endif

#include "../include/Greedy.hh"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <utility>
#include <vector>



void Greedy::GreedySolver(void)
{
   int ch;
   for(size_t tx = 0; tx < in.NetworkSize(); tx++)
   {
      ch = BestCh4Tx(tx);
      out.AssignCh(tx,ch);
   }
   out.TotalCostCheck();
}

void Greedy::DegreeSolver(void)
{
   std::vector<std::pair<int,unsigned>> degree_vec; //<degree , tx>

   for (size_t tx = 0; tx < in.NetworkSize(); tx++)
      degree_vec.push_back(std::pair<int, unsigned> {in.Degree(tx) , tx});

   std::sort(degree_vec.rbegin(),degree_vec.rend());

   int ch;
   unsigned tx;
   for(size_t i= 0; i < in.NetworkSize(); i++)
   {
      tx = degree_vec[i].second;
      ch = BestCh4Tx(tx);
      out.AssignCh(tx,ch);
   }
   out.TotalCostCheck();
}

void Greedy::DSaturSolver(void)
{
   int tx, ch;
   std::vector<std::vector<bool>> mat_blk_ch = in.MatBlkCh();
   std::vector<unsigned> satur_vec(in.NetworkSize(),0);

   for(size_t i = 0; i < in.NetworkSize(); i++)
      satur_vec[i] = std::count(mat_blk_ch[i].begin(),mat_blk_ch[i].end(),true);

   while ((tx = MaxSatur(satur_vec)) != -1)
   {
      ch = BestCh4Tx(tx);
      out.AssignCh(tx,ch);
      UpdateSatur(satur_vec,mat_blk_ch,tx,in.AdjTxFrom(tx));
   }
   out.TotalCostCheck();
}

int Greedy::BestCh4Tx(unsigned tx) const
{
   int best_ch = -1;
   Cost best_ch_cost, ch_cost;
   for(int ch = 0; ch < in.TotCh(); ch++)
   {
      if (!in.ChBlocked(tx,ch))
      {
         ch_cost = out.ChCost(tx, ch);
         if(best_ch == -1 || best_ch_cost > ch_cost)
         {
            best_ch = ch;
            best_ch_cost = ch_cost;
         }
      }
   }

   return best_ch;
}

int Greedy::MaxSatur(const std::vector<unsigned>& satur_vec)
{
   int tx_max_dsat = -1;

   for(size_t tx = 0; tx < in.NetworkSize(); tx++)
      if( out.Ch(tx) == -1 )
         if(tx_max_dsat == -1 || satur_vec[tx] > satur_vec[tx_max_dsat] ||(satur_vec[tx] == satur_vec[tx_max_dsat] && in.Degree(tx) > in.Degree(tx_max_dsat)))
            tx_max_dsat=tx;

   return tx_max_dsat;
}

void Greedy::UpdateSatur(std::vector<unsigned>& satur_vec, std::vector<std::vector<bool>>& mat_blk_ch, int tx_updated, const std::vector<unsigned>& update_list)
{
   for(auto tx: update_list)
   {
      if (out.Ch(tx) == -1  && in.ChSep(tx_updated,tx)) //altrimenti è ignorato a priori nel calcolo del DSatur, il sep è per i valori che hanno solo interferenza non li considero che saturano il ch
      {
         int start = (out.Ch(tx_updated) - in.ChSep(tx_updated,tx) + 1 >= 0) ? out.Ch(tx_updated) - in.ChSep(tx_updated,tx) + 1 : 0;
         int end = (out.Ch(tx_updated) + in.ChSep(tx_updated,tx) - 1 < in.TotCh()) ? out.Ch(tx_updated) + in.ChSep(tx_updated,tx) - 1 : in.TotCh() - 1;

         for(int ch = start; ch <= end; ch++)
            mat_blk_ch[tx][ch] = true;

         satur_vec[tx] = std::count(mat_blk_ch[tx].begin(),mat_blk_ch[tx].end(),true);

      }
   }
}

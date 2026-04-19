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
   assert(out.ValidSolution());
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
   assert(out.ValidSolution());
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
   assert(out.ValidSolution());
}

void Greedy::RegretSolver(void)
{
   //I don´t belive it is exactly like the classic concept of regret, I need a better classification or name
   //it is more like that feeling we get on sale that we buy more because we se a bigger % OFF rather than anything else
   std::vector<std::vector<Cost>> matrix_cost(in.NetworkSize(),std::vector<Cost>(in.TotCh()));
   std::vector<Cost> vector_delta_cost(in.NetworkSize());
   std::vector<unsigned> vector_satur(in.NetworkSize());
   int tx, ch;

   //Initialize saturation
   for (tx = 0; tx < static_cast<int>(in.NetworkSize()); ++tx)
      vector_satur[tx] = std::count(in.MatBlkCh()[tx].begin(),in.MatBlkCh()[tx].end(),true);

   //greedy algorithm
   while ((tx = NextTxRegret(vector_delta_cost,vector_satur)) != -1)
   {
      ch = BestCh4Tx(tx);
      out.AssignCh(tx,ch);
      UpdateRegretData(matrix_cost, vector_delta_cost, vector_satur, tx, in.AdjTxFrom(tx));
   }

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

int Greedy::MaxSatur(const std::vector<unsigned>& satur_vec) const
{
   int tx_max_dsat = -1;

   for(size_t tx = 0; tx < in.NetworkSize(); tx++)
      if( out.Ch(tx) == -1 )
         if(tx_max_dsat == -1 || satur_vec[tx] > satur_vec[tx_max_dsat] ||(satur_vec[tx] == satur_vec[tx_max_dsat] && in.Degree(tx) > in.Degree(tx_max_dsat)))
            tx_max_dsat=tx;

   return tx_max_dsat;
}

int Greedy::NextTxRegret(const std::vector<Cost>& vector_delta_cost, const std::vector<unsigned>& vector_satur) const
{
   int next_tx = -1;

   for(size_t tx = 0; tx < in.NetworkSize(); tx++)
      if(out.Ch(tx) == -1 ) // consider only unassigned tx
         if(next_tx == -1 ||//FIRST VALID CHANNEL
            vector_delta_cost[tx] > vector_delta_cost[next_tx] || //greater regret
            (vector_delta_cost[tx] == vector_delta_cost[next_tx] && vector_satur[tx] > vector_satur[next_tx]) ||//greater saturaton
            (vector_delta_cost[tx] == vector_delta_cost[next_tx] && vector_satur[tx] == vector_satur[next_tx] && in.Degree(tx) > in.Degree(next_tx))) //greater degree
            next_tx = tx;
   return next_tx;
}

void Greedy::UpdateSatur(std::vector<unsigned>& satur_vec, std::vector<std::vector<bool>>& mat_blk_ch, int tx_updated, const std::vector<unsigned>& update_list) const
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

void Greedy::UpdateRegretData(std::vector<std::vector<Cost>>& matrix_cost, std::vector<Cost>& vector_delta_cost, std::vector<unsigned>& vector_satur, int tx_updated, const std::vector<unsigned>& update_list) const
{
   unsigned new_satur;
   Cost min_cost, max_cost;
   bool first_valid_found;

   for(auto tx: update_list)
   {
      if (out.Ch(tx) == -1)
      {

         new_satur = 0;
         first_valid_found = false;

         for(int ch = 0; ch < in.TotCh(); ++ch)
         {
            //first update cost
            matrix_cost[tx][ch] = out.ChCost(tx, ch);

            //second use the new value for the rest
            if(in.ChBlocked(tx,ch))
            {
               //a blocked channel only increse satur
               new_satur++;
            }
            else
            {
               // a normal channel increse saturation only if the hard cost is not zero
               if (matrix_cost[tx][ch] >= Cost(1,0))
                  new_satur++;

               //and can be consider for max and min
               if (first_valid_found)
               {
                  if(min_cost > matrix_cost[tx][ch])
                     min_cost = matrix_cost[tx][ch];

                  if (max_cost < matrix_cost[tx][ch])
                     max_cost = matrix_cost[tx][ch];
               }
               else
               {
                  min_cost = max_cost = matrix_cost[tx][ch];
                  first_valid_found = true;
               }
            }

         }

         assert(first_valid_found); //only here for debug

         //all the ch possible cost were evaluated, now update delta
         vector_delta_cost[tx] = max_cost - min_cost;

      }
   }
}

#include "../include/Greedy.hh"
#include <algorithm>
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
   out.WriteCost(TotalCost());
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
   out.WriteCost(TotalCost());
}

int Greedy::BestCh4Tx(unsigned tx) const
{
   int best_ch = -1;
   Cost best_ch_cost, ch_cost;
   for(int ch = 0; ch < in.TotCh(); ch++)
   {
      if (!in.ChBlocked(tx,ch))
      {
         ch_cost = ChCostPreview(tx, ch);
         if(best_ch == -1 || best_ch_cost > ch_cost)
         {
            best_ch = ch;
            best_ch_cost = ch_cost;
         }
      }
   }
   return best_ch;
}

Cost Greedy::ChCostPreview(unsigned tx, int ch) const
{
   Cost tot_cost;
   for(auto t:in.AdjTxFrom(tx))
   {
      if (out.Ch(t) != -1)
      {
         if (in.ChSep(tx, t) && abs(ch - out.Ch(t)) < in.ChSep(tx, t))
            tot_cost += Cost(1);

         if(ch == out.Ch(t))
            tot_cost += Cost(0, in.SameChInt(tx,t));

         if(abs(ch - out.Ch(t)) == 1)
            tot_cost += Cost(0, in.AdjChInt(tx,t));
      }
   }

   for(auto t:in.AdjTxTo(tx))
   {
      if (out.Ch(t) != -1)
      {
         if (in.ChSep(t, tx) && abs(ch - out.Ch(t)) < in.ChSep(t, tx))
            tot_cost += Cost(1);

         if(ch == out.Ch(t))
            tot_cost += Cost(0, in.SameChInt(t,tx));

         if(abs(ch - out.Ch(t)) == 1)
            tot_cost += Cost(0, in.AdjChInt(t,tx));
      }
   }
   return tot_cost;
}

Cost Greedy::TotalCost(void)
{
   Cost tot_cost;

   for (unsigned tx = 0; tx < in.NetworkSize(); tx++)
   {
      for(auto t:in.AdjTxTo(tx))
      {
         if (out.Ch(t) != -1)
         {
            if (in.ChSep(t, tx) && abs(out.Ch(tx) - out.Ch(t)) < in.ChSep(t, tx))
               tot_cost += Cost(1);

            if(out.Ch(tx) == out.Ch(t))
               tot_cost += Cost(0, in.SameChInt(t,tx));

            if(abs(out.Ch(tx)- out.Ch(t)) == 1)
               tot_cost += Cost(0, in.AdjChInt(t,tx));
         }
      }
   }

   return tot_cost;
}

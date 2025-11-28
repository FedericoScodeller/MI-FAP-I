#include "../include/Output.hh"
#include <algorithm>
#include <cstdlib>



Output::Output(const Input& in)
       :input(in)
{
   vec_ch.resize(input.NetworkSize(),-1);
   mat_cost.resize(input.NetworkSize(),std::vector<Cost>(input.TotCh()));
}


int Output::MinCostCh (unsigned tx) const
{
   int ch_min = -1;

   for (size_t ch = 0; ch < input.TotCh(); ch++)
      if(!input.ChBlocked(tx,ch) && (ch_min == -1 || mat_cost[tx][ch]<mat_cost[tx][ch_min]))
         ch_min=ch;
   return ch_min;
}

void Output::AssignCh(unsigned tx, unsigned ch)
{
   vec_ch[tx]=ch;
   unsigned f_start, f_stop;
   for(auto t: input.AdjTx(tx))
   {
      //HANDOVE
      f_start = std::max(0,static_cast<int>(ch) - static_cast<int>(input.ChSep(tx,t)) + 1);
      f_stop = std::min(input.TotCh() - 1,ch + input.ChSep(tx,t) -1);
      for(size_t f = f_start; f <= f_stop;f++)
         mat_cost[t][f]+=Cost(1); //it shuold be inplicity but i will probably forget

      // SAME
      mat_cost[t][ch]+=Cost(0,input.SameChInt(tx,t));

      // ADJ
      if(static_cast<int>(ch)-1 >= 0)
         mat_cost[t][ch - 1]+=Cost(0,input.AdjChInt(tx,t));

      if(ch+1 < input.TotCh())
         mat_cost[t][ch + 1]+=Cost(0,input.AdjChInt(tx,t));

   }
}

Cost Output::TotCost(void) const
{
   Cost tot_cost;
   for(size_t i = 0; i < input.NetworkSize(); i++)
   {
      if(mat_cost[i][vec_ch[i]].Hard() > 0)
         tot_cost+=mat_cost[i][vec_ch[i]].Hard();
      else
         tot_cost+=mat_cost[i][vec_ch[i]];
   }
   return tot_cost;
}

Cost Output::TotCostAlt(void) const
{
   Cost tot_cost;
   for(size_t i = 0; i < input.NetworkSize(); i++)
   {
      for(size_t j = i + 1; j < input.NetworkSize(); j++)
      {
         if (input.ChSep(i,j) > std::abs(vec_ch[i]-vec_ch[j])) {
            tot_cost+=1;
         }
         else if (vec_ch[i]==vec_ch[j]) {
            tot_cost+=Cost(0,input.SameChInt(i,j));
         }
         else if (std::abs(vec_ch[i]-vec_ch[j])==1) {
            tot_cost+=Cost(0,input.AdjChInt(i,j));
         }
      }
   }
   return tot_cost;
}


std::ostream &operator<<(std::ostream &os, const Output &out)
{

   os << "Assigned ch: [ ";
   for(auto ch : out.vec_ch)
      os << ch << " ";
   os << "]" << std::endl;

   os << "Channel cost: " << std::endl << "  ";
   for(size_t i = 0; i < out.input.NetworkSize(); i++)
   {
      for(size_t j = 0; j < out.input.TotCh(); j++)
      {
         os << out.mat_cost[i][j] << " ";
      }
      os << std::endl << "  ";
   }


   return os;
}

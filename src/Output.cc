#include "../include/Output.hh"
#include <cassert>


Output::Output(const Input& in)
       :input(in)
{
   vec_ch.resize(input.NetworkSize(),-1);
}

Cost Output::ChCost(unsigned tx, int ch) const
{
   Cost tot_cost;
   for(auto t:input.AdjTxFrom(tx))
   {
      if (vec_ch[t] != -1)
      {
         if (input.ChSep(tx, t) && abs(ch - vec_ch[t]) < input.ChSep(tx, t))
            tot_cost += Cost(1);

         if(ch == vec_ch[t])
            tot_cost += Cost(0, input.SameChInt(tx,t));

         if(abs(ch - vec_ch[t]) == 1)
            tot_cost += Cost(0, input.AdjChInt(tx,t));
      }
   }

   for(auto t:input.AdjTxTo(tx))
   {
      if (vec_ch[t] != -1)
      {
         if (input.ChSep(t, tx) && abs(ch - vec_ch[t]) < input.ChSep(t, tx))
            tot_cost += Cost(1);

         if(ch == vec_ch[t])
            tot_cost += Cost(0, input.SameChInt(t,tx));

         if(abs(ch - vec_ch[t]) == 1)
            tot_cost += Cost(0, input.AdjChInt(t,tx));
      }
   }
   return tot_cost;
}

Cost Output::SolutionCost(void) const
{
   Cost solution_cost;
   for (unsigned tx = 0; tx < input.NetworkSize(); tx++)
   {
      if (vec_ch[tx] != -1) {
         for(auto t:input.AdjTxTo(tx))
         {
            if (vec_ch[t] != -1)
            {
               if (input.ChSep(t, tx) && abs(vec_ch[tx] - vec_ch[t]) < input.ChSep(t, tx))
                  solution_cost += Cost(1);

               if(vec_ch[tx] == vec_ch[t])
                  solution_cost += Cost(0, input.SameChInt(t,tx));

               if(abs(vec_ch[tx]- vec_ch[t]) == 1)
                  solution_cost += Cost(0, input.AdjChInt(t,tx));
            }
         }

      }
   }
   return solution_cost;
}

void Output::Reset(void)
{
   for(auto& tx:vec_ch)
      tx=-1;
}

bool Output::ValidSolution(void) const
{
   for(unsigned tx = 0; tx < input.NetworkSize(); tx++)
      if (vec_ch[tx] != -1)
         if(input.ChBlocked(tx,vec_ch[tx]))
            return false;

   return true;
}

Output& Output::operator=(const Output& out)
{
   vec_ch = out.vec_ch;
   return *this;
}

std::ostream &operator<<(std::ostream &os, const Output &out)
{
   os << "Total cost: " << out.SolutionCost() << std::endl;

   #ifdef DEBUG
   os << "Assigned ch: [ ";
   for(auto ch : out.vec_ch)
      os << ch << " ";
   os << "]" << std::endl;
   #endif

   return os;
}

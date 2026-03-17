#include "../include/Output.hh"
#include <cassert>


Output::Output(const Input& in)
       :input(in)
{
   vec_ch.resize(input.NetworkSize(),-1);
}

void Output::AssignCh(unsigned tx, unsigned ch)
{
   if (vec_ch[tx] == static_cast<int>(ch))
       return;

   if(vec_ch[tx] != -1)
      RemoveCh(tx);

   vec_ch[tx]=ch;
   tot_cost += ChCost(tx,ch);
}

void Output::RemoveCh(unsigned tx)
{
   if(vec_ch[tx] == -1)
      return;

   tot_cost -= ChCost(tx,vec_ch[tx]);
   vec_ch[tx]=-1;

}

void Output::AssignChCost(unsigned tx, unsigned ch, Cost cost)
{
   if (vec_ch[tx] == static_cast<int>(ch))
       return;

   if(vec_ch[tx] != -1)
      RemoveCh(tx);

   vec_ch[tx]=ch;
   tot_cost += cost;
}

void Output::RemoveChCost(unsigned tx, Cost cost)
{
   if(vec_ch[tx] == -1)
      return;

   tot_cost -= cost;
   vec_ch[tx]=-1;

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

void Output::TotalCostCheck(void) const
{
   Cost cost_check;

   assert(this->ValidSolution());

   for (unsigned tx = 0; tx < input.NetworkSize(); tx++)
   {
      for(auto t:input.AdjTxTo(tx))
      {
         if (vec_ch[t] != -1)
         {
            if (input.ChSep(t, tx) && abs(vec_ch[tx] - vec_ch[t]) < input.ChSep(t, tx))
               cost_check += Cost(1);

            if(vec_ch[tx] == vec_ch[t])
               cost_check += Cost(0, input.SameChInt(t,tx));

            if(abs(vec_ch[tx]- vec_ch[t]) == 1)
               cost_check += Cost(0, input.AdjChInt(t,tx));
         }
      }
   }

   assert(cost_check == tot_cost);
}

void Output::Reset(void)
{
   for(auto& tx:vec_ch)
      tx=-1;

   tot_cost = 0;
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
   tot_cost = out.tot_cost;
   return *this;
}

std::ostream &operator<<(std::ostream &os, const Output &out)
{
   os << "Total cost: " << out.tot_cost << std::endl;
   os << "Assigned ch: [ ";
   for(auto ch : out.vec_ch)
      os << ch << " ";
   os << "]" << std::endl;

   return os;
}

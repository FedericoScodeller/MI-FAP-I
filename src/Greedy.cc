#include "../include/Greedy.hh"

void GreedyBasic(const Input& in, Output& out)
{
   int ch;
   for(size_t tx = 0; tx < in.NetworkSize(); tx++)
   {
      //add gestione errore
      ch=out.MinCostCh(tx);
      out.AssignCh(tx ,ch);
   }
}

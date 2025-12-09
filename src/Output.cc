#include "../include/Output.hh"


Output::Output(const Input& in)
       :input(in)
{
   vec_ch.resize(input.NetworkSize(),-1);
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

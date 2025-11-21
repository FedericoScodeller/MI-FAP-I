#include "../include/Output.hh"



Output::Output(const Input& in)
       :input(in)
{
   vec_ch.resize(input.NetworkSize(),-1);
   vec_cost.resize(input.NetworkSize());//do I need to use an 0, 0.0 cost item as a new value?
}

std::ostream &operator<<(std::ostream &os, const Output &out)
{

   os << "Assigned ch: [ ";
   for(auto ch : out.vec_ch)
      os << ch << " ";
   os << "]" << std::endl;

   os << "Channel cost: [ ";
   for(auto cost : out.vec_cost)
      os << cost << " ";
   os << "]" << std::endl;


   return os;
}

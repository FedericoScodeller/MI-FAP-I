#include "Cost.hh"

std::ostream &operator<<(std::ostream &os, const Cost &cost)
{
   #ifdef DEBUG
   os << cost.hard << " hard link broken & "
      << cost.interf << " interference" << std::endl;
   #endif

   os << cost.hard * cost.HARD_TO_INTERF + cost.interf;

   return os;
}


Cost& Cost::operator+=(const Cost& c)
{
   this->hard+=c.hard;
   this->interf+=c.interf;
   return *this;
}

Cost& Cost::operator=(const Cost& c)
{
   this->hard=c.hard;
   this->interf=c.interf;
   return *this;
}

bool operator==(const Cost& a,const Cost& b)
{
   return a.hard == b.hard && a.interf > b.interf - b.TOLLERANCE && a.interf < b.interf + b.TOLLERANCE;
}

bool operator<(const Cost& a,const Cost& b)
{
   return a.hard < b.hard ||(a.hard == b.hard && a.interf < b.interf);
}

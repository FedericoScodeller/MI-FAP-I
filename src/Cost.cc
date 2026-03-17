#include "../include/Cost.hh"


std::ostream &operator<<(std::ostream &os, const Cost &cost)
{
   os <<"[" <<cost.hard << "](" << cost.interf  << ")";
   return os;
}

Cost& Cost::operator+=(const Cost& c)
{
   this->hard+=c.hard;
   this->interf+=c.interf;
   return *this;
}

Cost& Cost::operator-=(const Cost& c)
{
   this->hard-=c.hard;
   this->interf-=c.interf;
   return *this;
}

Cost& Cost::operator=(const Cost& c)
{
   this->hard=c.hard;
   this->interf=c.interf;
   return *this;
}

bool operator<(const Cost& a,const Cost& b)
{
   return a.hard < b.hard ||(a.hard == b.hard && a.interf < b.interf);
}

bool operator>(const Cost& a,const Cost& b)
{
   return a.hard > b.hard ||(a.hard == b.hard && a.interf > b.interf);
}

bool operator<=(const Cost& a,const Cost& b)
{
   return a.hard < b.hard ||(a.hard == b.hard && a.interf <= b.interf);
}

bool operator>=(const Cost& a,const Cost& b)
{
   return a.hard > b.hard ||(a.hard == b.hard && a.interf >= b.interf);
}


bool operator==(const Cost& a,const Cost& b)
{
   return a.hard == b.hard && a.interf == b.interf;
}

Cost operator+(const Cost& a,const Cost& b)
{
    return Cost(a.hard + b.hard, a.interf + b.interf);
}

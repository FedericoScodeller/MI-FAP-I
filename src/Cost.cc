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
   this->hard-=c.hard; //non mi piace teoricamente non fallisce mai ma il tipo non lo garantisce int mi toglierebbe casini
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

bool operator==(const Cost& a,const Cost& b)
{
   //I am going to nuke it the moment I am sure i dont need it
   return a.hard == b.hard && std::abs(a.interf - b.interf) < 0.00001;
}

#ifndef COST_HH
#define COST_HH
#include <iostream>

class Cost
{
   friend std::ostream &operator<<(std::ostream &os, const Cost &cost);
   friend bool operator<(const Cost& a,const Cost& b);
   friend bool operator>(const Cost& a,const Cost& b);
   friend bool operator<=(const Cost& a,const Cost& b);
   friend bool operator>=(const Cost& a,const Cost& b);
   friend bool operator==(const Cost& a,const Cost& b);
   friend Cost operator+(const Cost& a,const Cost& b);

   public:
      int Hard(void) const {return hard;};
      int Interf(void) const {return interf;};

      Cost(int hard=0, int interf=0):hard(hard),interf(interf){};
      Cost& operator+=(const Cost& c);
      Cost& operator-=(const Cost& c);
      Cost& operator=(const Cost& c);

   protected:

      int hard;
      int interf;



};

#endif

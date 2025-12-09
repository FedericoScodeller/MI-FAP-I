#ifndef COST_HH
#define COST_HH
#include <iostream>

class Cost
{
   friend std::ostream &operator<<(std::ostream &os, const Cost &cost);
   friend bool operator<(const Cost& a,const Cost& b);
   friend bool operator>(const Cost& a,const Cost& b);
   friend bool operator==(const Cost& a,const Cost& b);//to trash immediatly

   public:
      unsigned Hard(void) const {return hard;};

      Cost(int hard=0, float interf=0.0):hard(hard),interf(interf){};
      Cost& operator+=(const Cost& c);
      Cost& operator-=(const Cost& c);
      Cost& operator=(const Cost& c);

   protected:

      int hard;
      float interf;



};

#endif

#ifndef COST_HH
#define COST_HH
#include <iostream>

class Cost
{
   protected:
      const float TOLLERANCE = 0.0001;
      const unsigned HARD_TO_INTERF = 1000;
      unsigned hard;
      float interf;

   public:
      Cost(unsigned hard=0, float interf=0.0):hard(hard),interf(interf){};
      Cost& operator+=(const Cost& c);
      Cost& operator=(const Cost& c);
//do I need the overload for =?
      friend std::ostream &operator<<(std::ostream &os, const Cost &cost);
      friend bool operator==(const Cost& a,const Cost& b);
      friend bool operator<(const Cost& a,const Cost& b);
      //is needed operator+?
};

#endif

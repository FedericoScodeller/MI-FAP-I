#ifndef GREEDY_HH
#define GREEDY_HH

#include "Cost.hh"
#include "Input.hh"
#include "Output.hh"

class Greedy
{
   public:
      Greedy(const Input& in):in(in),out(Output(in)){};

      Output Solution(void) const {return out;};
      Cost TotalCost(void);

      void GreedySolver(void);
      void DegreeSolver(void);




   protected:
      const Input& in;
      Output out;

      int BestCh4Tx(unsigned tx) const;
      Cost ChCostPreview(unsigned tx, int ch) const;

};

#endif

#ifndef OUTPUT_HH
#define OUTPUT_HH
#include "Cost.hh"
#include "Input.hh"

class Output
{

   friend std::ostream &operator<<(std::ostream &os, const Output &out);

   public:
      Output(const Input& in);
      void AssignCh(unsigned tx, unsigned ch){vec_ch[tx]=ch;};
      void WriteCost(Cost c) {tot_cost=c;};
      Cost TotCost(void) const {return tot_cost;};
      int Ch(int tx) const {return vec_ch[tx];};

   protected:
      const Input& input;
      std::vector<int> vec_ch;
      Cost tot_cost;




};

#endif

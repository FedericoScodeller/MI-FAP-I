#ifndef OUTPUT_HH
#define OUTPUT_HH
#include "Cost.hh"
#include "Input.hh"

class Output
{

   friend std::ostream &operator<<(std::ostream &os, const Output &out);

   public:
      Output(const Input& in);
      int MinCostCh (unsigned tx) const;
      void AssignCh(unsigned tx, unsigned ch);
      Cost TotCost(void) const;
      Cost TotCostAlt(void) const;

   protected:
      const Input& input;
      std::vector<int> vec_ch;
      std::vector<std::vector<Cost>> mat_cost;




};

#endif

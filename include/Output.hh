#ifndef OUTPUT_HH
#define OUTPUT_HH
#include "Cost.hh"
#include "Input.hh"

class Output
{

   friend std::ostream &operator<<(std::ostream &os, const Output &out);

   public:
      Output(const Input& in);

      void AssignCh(unsigned tx, unsigned ch);
      void RemoveCh(unsigned tx);

      Cost TotCost(void) const {return tot_cost;};
      int Ch(int tx) const {return vec_ch[tx];};

      void Reset(void);

      Cost ChCost(unsigned tx, int ch) const;
      void TotalCostCheck(void) const;

   protected:
      const Input& input;
      std::vector<int> vec_ch;
      Cost tot_cost;




};

#endif

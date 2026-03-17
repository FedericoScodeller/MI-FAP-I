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
      void AssignChCost(unsigned tx, unsigned ch, Cost cost);
      void RemoveChCost(unsigned tx, Cost cost);

      Cost TotCost(void) const {return tot_cost;};
      int Ch(int tx) const {return vec_ch[tx];};

      void Reset(void);

      Cost ChCost(unsigned tx, int ch) const;
      void TotalCostCheck(void) const;
      bool ValidSolution(void) const;

      Output& operator=(const Output& out);

   protected:
      const Input& input;
      std::vector<int> vec_ch;
      Cost tot_cost;

};

#endif

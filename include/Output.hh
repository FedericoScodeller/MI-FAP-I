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
      void RemoveCh(unsigned tx){vec_ch[tx]=-1;};

      int Ch(int tx) const {return vec_ch[tx];};

      void Reset(void);

      Cost ChCost(unsigned tx, int ch) const;
      Cost SolutionCost(void) const;
      bool ValidSolution(void) const;

      Output& operator=(const Output& out);

   protected:
      const Input& input;
      std::vector<int> vec_ch;

};

#endif

#ifndef OUTPUT_HH
#define OUTPUT_HH
#include "Cost.hh"
#include "Input.hh"

class Output
{
   protected:
      const Input& input;
      std::vector<int> vec_ch;
      std::vector<Cost> vec_cost;
   public:
      Output(const Input& in);

   friend std::ostream &operator<<(std::ostream &os, const Output &out);
};

#endif

#ifndef WL_BACKTRACKING_HH
#define WL_BACKTRACKING_HH
#include "../libs/BacktrackingOpt.hh"
#include "Input.hh"
#include "Output.hh"
#include "Cost.hh"

class BacktrackingMIFAPOpt : public BacktrackingOpt<Input,Output, Cost>
{
 public:
  BacktrackingMIFAPOpt(const Input& in, unsigned fl) : BacktrackingOpt(in,fl) {}
 protected:
  // hot-spots
  void FirstOfLevel() override;
  bool NextOfLevel() override;
  bool Feasible() override;
  ::Cost Cost() override;
  // cold-spots
  bool NonImprovingBranch() override;
  void GoUpOneLevel() override;
};
#endif

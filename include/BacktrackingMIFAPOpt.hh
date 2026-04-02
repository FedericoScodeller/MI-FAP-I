#ifndef WL_BACKTRACKING_HH
#define WL_BACKTRACKING_HH
#include "../libs/BacktrackingOpt.hh"
#include "Input.hh"
#include "Output.hh"
#include "Cost.hh"
#include <vector>

class BacktrackingMIFAPOpt : public BacktrackingOpt<Input,Output, Cost>
{
 public:
  BacktrackingMIFAPOpt(const Input& in, unsigned fl);
  bool SearchTimed(unsigned timer_sec);
  


  //TELEMETRY
  unsigned long long FullSolutionExaminated(void) const {return full_solution_examinated;};
  unsigned long long NodesSkippedPerLevel(unsigned n) const {return nodes_skipped_per_level[n];};
  int Level() const {return level;};
  int VecChIndex(int n) const {return vec_ch_index[n];};

 protected:
  // hot-spots
  void FirstOfLevel() override;
  bool NextOfLevel() override;
  bool Feasible() override;
  class Cost Cost() override;
  // cold-spots
  bool NonImprovingBranch() override;
  void GoUpOneLevel() override;

  bool FullSolution() override;

  std::vector<std::vector<std::pair<class Cost,int>>> mat_ch_cost;
  std::vector<int> vec_ch_index;
  std::vector<std::pair<int, unsigned>> vec_tx_order; //<degree , tx>

  //TELEMETRY
  unsigned long long full_solution_examinated;
  std::vector<unsigned long long> nodes_skipped_per_level;
};
#endif

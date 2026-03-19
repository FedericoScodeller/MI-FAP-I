#ifndef BACKTRACKING_HH
#define BACKTRACKING_HH
#include "Input.hh"
#include "Output.hh"
#include "Cost.hh"
#include <utility>
#include <vector>



class Backtracking
{
   public:
      Backtracking(const Input& in);
      Output BestSolution() const {return best;}
      bool TimerSearch(unsigned timer_sec);
   protected:
      void Down();
      void Up();
      bool AtRoot() const { return level == -1; }
      bool Solution() const { return level == final_level; }
      bool Improvement() const{return out.TotCost() < best.TotCost();};
      Cost SolutionCost() const{return best.TotCost();};

      const Input& in;
      int level, final_level;
      Output out, best;

      std::vector<std::pair<int, unsigned>> vec_tx; //<degree , tx>
      std::vector<int> level_ch_index; //per ciascun lv ricorda a che entrata della matrice sono
      std::vector<std::vector<std::pair<Cost, unsigned>>> bck_trk_mtr;
};


#endif

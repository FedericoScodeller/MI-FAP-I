#ifndef BACKTRACKING_HH
#define BACKTRACKING_HH
#include <iostream>

using namespace std;

template <typename Input, typename Output, typename CFtype>
class BacktrackingOpt
{
public:
  BacktrackingOpt(const Input& i, unsigned fl);
  bool Search();
  Output BestSolution() { return best; }
  unsigned NumNodes() const { return count; }
protected:
  virtual void GoDownOneLevel() { level++; }
  virtual void GoUpOneLevel(){ level--; }
  virtual void FirstOfLevel() = 0;
  virtual bool NextOfLevel() = 0;
  virtual bool AtRootLevel() { return level == -1; }
  virtual bool FullSolution() { return level == final_level; }
  virtual bool Feasible() = 0;
  virtual bool NonImprovingBranch() { return false; }
  virtual CFtype Cost() = 0;
  int level, final_level;
  unsigned count;
  CFtype cost, best_cost;
  const Input& in;
  Output out, best;
};

template <typename Input, typename Output, typename CFtype>
BacktrackingOpt<Input,Output,CFtype>::BacktrackingOpt(const Input& i, unsigned fl)
  : in(i), out(in), best(in)
{
  count = 0;
  level = -1;
  final_level = fl;
}

template <typename Input, typename Output, typename CFtype>
bool BacktrackingOpt<Input,Output,CFtype>::Search()
{
  bool backtrack, first_sol_found = false;
   do
   {
      if (first_sol_found)
        backtrack = !Feasible() || NonImprovingBranch();
      else
        backtrack = !Feasible();

      if (backtrack)
      {
      do
      {
         if (NextOfLevel())
         {
            count++;
            backtrack = false;
         }
         else
            GoUpOneLevel();
      }
      while (backtrack && !AtRootLevel());
      }
      else
      {
         if (FullSolution())
         {
            cost = Cost();
            if (!first_sol_found || cost < best_cost)
            {

               if (first_sol_found)
                  cerr << "New best solution " << out
                     << " (current cost " << cost << ", previous " << best_cost << ")" << endl;
               else
                  cerr << "First solution " << out
                     << " (current cost " << cost << ") " << endl;
               first_sol_found = true;
               best_cost = cost;
               best = out;
            }
            backtrack = true; // continue search
         }
         else
         {
            GoDownOneLevel();
            FirstOfLevel();
            count++;
         }
      }
   }
   while (!AtRootLevel());

   return first_sol_found;
}
#endif

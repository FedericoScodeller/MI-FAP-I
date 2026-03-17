#include "../include/BacktrackingMIFAPOpt.hh"

void BacktrackingMIFAPOpt::FirstOfLevel()
{
   out.AssignCh(level,0);
}

bool BacktrackingMIFAPOpt::NextOfLevel()
{
   int next_ch = out.Ch(level) + 1;
   if (next_ch < in.TotCh())
   {
      out.AssignCh(level,next_ch);
      return true;
   }
   return false;
}

bool BacktrackingMIFAPOpt::Feasible()
{
  return out.ValidSolution();
}

Cost BacktrackingMIFAPOpt::Cost()
{
   return out.TotCost();
}

bool BacktrackingMIFAPOpt::NonImprovingBranch()
{
  return out.TotCost() >= best_cost;
}

void BacktrackingMIFAPOpt::GoUpOneLevel()
{
   out.RemoveCh(level--);
}

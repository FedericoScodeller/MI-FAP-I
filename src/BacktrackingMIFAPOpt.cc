#include "../include/BacktrackingMIFAPOpt.hh"

void BacktrackingMIFAPOpt::FirstOfLevel()
{
   for(int first_ch = out.Ch(level) + 1; first_ch < in.TotCh(); ++first_ch){
      if (!in.ChBlocked(level,first_ch)) {
         out.AssignCh(level,first_ch);
         break;
      }
   }
}

bool BacktrackingMIFAPOpt::NextOfLevel()
{
   for(int next_ch = out.Ch(level) + 1; next_ch < in.TotCh(); ++next_ch){
      if (!in.ChBlocked(level,next_ch)) {
         out.AssignCh(level,next_ch);
         return true;
      }
   }
   return false;
}

bool BacktrackingMIFAPOpt::Feasible()
{
   //this version assumed valid only ch assigment... non sono un fan? meglio non farlo? chiedi al prof
   return true;
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

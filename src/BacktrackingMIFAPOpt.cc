#include "../include/BacktrackingMIFAPOpt.hh"
#include <algorithm>
#include <cassert>
#include <ostream>
#include <utility>
#include <vector>

BacktrackingMIFAPOpt::BacktrackingMIFAPOpt(const Input& in, unsigned fl)
                     : BacktrackingOpt(in,fl),
                       mat_ch_cost(in.NetworkSize(),std::vector<std::pair<class Cost, int>>(in.TotCh())),
                       vec_ch_index(in.NetworkSize(),-1)
{}

void BacktrackingMIFAPOpt::FirstOfLevel()
{
   int tx = level;


   for(int ch = 0; ch < in.TotCh(); ++ch)
      mat_ch_cost[level][ch]={out.ChCost(tx,ch),ch};


   std::sort(mat_ch_cost[level].begin(),mat_ch_cost[level].end());


   vec_ch_index[level] = 0;
   out.AssignCh(tx,mat_ch_cost[level][vec_ch_index[level]].second);
   cost += mat_ch_cost[level][vec_ch_index[level]].first;

   #ifdef DEBUG_BACKTRACKING
   cerr << "Ch cost vector pair of lv " <<level<<endl;
   for(int n = 0; n < in.TotCh(); ++n)
      cerr << "{" << mat_ch_cost[level][n].first <<", " <<mat_ch_cost[level][n].second<<"}" <<endl;
   if(!(cost == out.SolutionCost()))
   {
      cerr << "abort in FirstOfLevel\n";
      cerr << "cost: " << cost << endl;
      cerr << "out.cost: " << out.SolutionCost() << endl;
   }
   assert(cost == out.SolutionCost());
   #endif

}

bool BacktrackingMIFAPOpt::NextOfLevel()
{
   // se non mi fermo prima del -1 vado fuori memoria, inutile se uso l`uscita anticipata
   if (vec_ch_index[level] < in.TotCh() -1 ) {
      //qui ho avuto vari bug logici se tolgo prima rischi di avere casi in cui l'UpOne toglie due volte la stessa misura
      // ORDINE OP: togli old ch cost -> assegna il NUOVO CANALE -> Aggiungi il nuovo costo

      cost -= mat_ch_cost[level][vec_ch_index[level]].first;

      int tx = level;
      int ch = mat_ch_cost[level][++vec_ch_index[level]].second;

      out.AssignCh(tx,ch);
      cost += mat_ch_cost[level][vec_ch_index[level]].first;

      #ifdef DEBUG_BACKTRACKING
      if(!(cost == out.SolutionCost()))
      {
         cerr << "abort in NextOfLevel\n";
         cerr << "level: " << level <<endl;
         cerr << "tx: " << tx << endl;
         cerr << "old ch: " << mat_ch_cost[level][vec_ch_index[level]-1].second << endl;
         cerr << "old ch index: " << vec_ch_index[level]-1<< endl;
         cerr << "old ch cost: " << mat_ch_cost[level][vec_ch_index[level]-1].first << endl;
         cerr << "new ch: " << ch << endl;
         cerr << "new ch index: " << vec_ch_index[level]<< endl;
         cerr << "new ch cost: " << mat_ch_cost[level][vec_ch_index[level]].first << endl;
         cerr << "cost: " << cost << endl;
         cerr << "out.cost: " << out.SolutionCost() << endl;
      }
      assert(cost == out.SolutionCost());
      return true; //questo è per testare che gestisca anche i casi in cui arriva in fondo, ho avuto dei bug che il caso anticipato non ha
      #endif
      //il -1 serve solo per sapere se inizializzato altrimenti ho un bug logico se nella prima catena ho anche un solo un valore non valido
      return !(NonImprovingBranch() && (best.Ch(0) != -1)); //essendo ordinati puoi abbandonarli prima

   }
   return false;
}

bool BacktrackingMIFAPOpt::Feasible()
{
   //if empty solution is of course valid, else just check the last added
   int tx = level;
   int ch = out.Ch(level);
   return level == -1 || !in.ChBlocked(tx, ch );
}

Cost BacktrackingMIFAPOpt::Cost()
{
   return cost;
}

bool BacktrackingMIFAPOpt::NonImprovingBranch()
{
  return cost >= best_cost;
}

void BacktrackingMIFAPOpt::GoUpOneLevel()
{
   int tx = level;
   int ch = vec_ch_index[level];

   cost -= mat_ch_cost[tx][ch].first;
   out.RemoveCh(tx);

   --level;

   #ifdef DEBUG_BACKTRACKING
   if(!(cost == out.SolutionCost()))
   {
      cerr << "abort in GoUpOneLevel\n";
      cerr << "cost: " << cost << endl;
      cerr << "out.cost: " << out.SolutionCost() << endl;
   }
   assert(cost == out.SolutionCost());
   #endif

}

bool BacktrackingMIFAPOpt::FullSolution()
{
   #ifdef DEBUG_BACKTRACKING
   if(!(cost == out.SolutionCost()))
   {
      cerr << "abort in FullSolution\n";
      cerr << "cost: " << cost << endl;
      cerr << "out.cost: " << out.SolutionCost() << endl;
   }
   assert(cost == out.SolutionCost());
   #endif

   return level == final_level;
}

bool BacktrackingMIFAPOpt::SearchTimed(unsigned timer_sec)
{
   bool backtrack, first_sol_found = false;

   auto start = std::chrono::steady_clock::now();
   auto now   = std::chrono::steady_clock::now();
   auto timer = std::chrono::seconds(timer_sec);

   do
   {
      //the old backtrack true after having found a solution was pointless it was always rewritten by this if-else
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
                  cerr << "New best solution " << out << "cost: "<< cost <<endl;
               else
               {
                  cerr << "First solution " << out << endl;
                  first_sol_found = true;
               }

               best_cost = cost;
               best = out;
            }
         }
         else
         {
            GoDownOneLevel();
            FirstOfLevel();
            count++;
         }
      }
      now = std::chrono::steady_clock::now();
   }
   while (!AtRootLevel() && (now - start < timer));

   return first_sol_found;
}

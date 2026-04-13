#include "../include/BacktrackingMIFAPOpt.hh"
#include <algorithm>
#include <cassert>
#include <ostream>
#include <utility>
#include <vector>

BacktrackingMIFAPOpt::BacktrackingMIFAPOpt(const Input& in, unsigned fl)
                     : BacktrackingOpt(in,fl),
                       mat_ch_cost(in.NetworkSize(),std::vector<std::pair<class Cost, int>>(in.TotCh())),
                       vec_ch_index(in.NetworkSize(),-1),
                       vec_tx_order(in.NetworkSize()),
                       nodes_skipped_per_level(in.NetworkSize(),0)

{
  //TELEMETRY
  full_solution_examinated = 0;
}

void BacktrackingMIFAPOpt::FirstOfLevel()
{
   unsigned tx = vec_tx_order[level];

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
   cerr << "first of level assiment is " <<level << ": " << out.Ch(tx) <<endl;
   #endif

}

bool BacktrackingMIFAPOpt::NextOfLevel()
{
   // se non mi fermo prima del -1 vado fuori memoria, inutile se uso l`uscita anticipata
  if (vec_ch_index[level] < in.TotCh() - 1 ) {
      //qui ho avuto vari bug logici se tolgo prima rischi di avere casi in cui l'UpOne toglie due volte la stessa misura
      // ORDINE OP: togli old ch cost -> assegna il NUOVO CANALE -> Aggiungi il nuovo costo
      int tx = vec_tx_order[level]; // DEGREE 
      int ch = mat_ch_cost[level][vec_ch_index[level]].second;

      cost -= mat_ch_cost[level][vec_ch_index[level]].first;
      ch = mat_ch_cost[level][++vec_ch_index[level]].second;
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
      //è uno spoco trucco logico che deve esssere migliorato magari con una semplice funzione out.CompleteSolution o migliorando la valid solution
      return !(NonImprovingBranch() && (best.Ch(0) != -1)); //essendo ordinati puoi abbandonarli prima
   }
   return false;
}

bool BacktrackingMIFAPOpt::Feasible()
{
   //if empty solution is of course valid, else just check the last added
   int tx = vec_tx_order[level]; 
   int ch = out.Ch(tx);

   return level == -1 || !in.ChBlocked(tx, ch);
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
   int tx = vec_tx_order[level];
   cost -= mat_ch_cost[level][vec_ch_index[level]].first;
   out.RemoveCh(tx);
   //vec_tx_order[--level] = -1; //pointless assigment i did it for no reason
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

void BacktrackingMIFAPOpt::GoDownOneLevel()
{
   unsigned tx, best_tx;
   int tx_sat, best_tx_sat, start, end;
   std::vector<bool> vec_ch_sat;
   bool first_sat_found = false;

   for(tx = 0; tx < in.NetworkSize(); ++tx)
   {
      //CHECK IF VALID TX
      if(out.Ch(tx) == -1)
      {
         //START CALC SAT
         vec_ch_sat = in.MatBlkCh()[tx];

         for(auto t : in.AdjTxTo(tx)) //questi sono i tramsemttitori che causano saturazione a tx, from t to tx
         {
            if(out.Ch(t) != -1 && in.ChSep(t,tx))
            {
               start = (out.Ch(t) - in.ChSep(t,tx) + 1 >= 0) ? out.Ch(t) - in.ChSep(t,tx) + 1 : 0;
               
               end = (out.Ch(t) + in.ChSep(t,tx) - 1 < in.TotCh()) ? out.Ch(t) + in.ChSep(t,tx) - 1 : in.TotCh() - 1;

               for(int ch = start; ch <= end; ++ch)
                  vec_ch_sat[ch] = true;
            }
         }
         //COUNT SAT CH FOR A TX
         tx_sat = std::count(vec_ch_sat.begin(),vec_ch_sat.end(),true);

         //CONFRONT WITH BEST IF AVAIALABLE
         if(first_sat_found)
         {
            if(tx_sat > best_tx_sat || ( tx_sat == best_tx_sat && in.Degree(tx) > in.Degree(best_tx) ))
            {
               best_tx = tx;
               best_tx_sat = tx_sat;
            }
         }
         else
         {
            first_sat_found = true;
            best_tx = tx;
            best_tx_sat = tx_sat;
         }
      }
   }

   assert(first_sat_found);

   vec_tx_order[++level] = best_tx;

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

      //ONLY FOR TELEMETRY
      if(!Feasible())
         ++nodes_skipped_per_level[level];
         

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
            {
               //FOR TELEMETRY
               if(NonImprovingBranch())
                  nodes_skipped_per_level[level] += in.TotCh() - vec_ch_index[level];
                  
               GoUpOneLevel();
            }
         }
         while (backtrack && !AtRootLevel());
      }
      else
      {
         if (FullSolution())
         {
            cost = Cost();
            ++full_solution_examinated;
            if (!first_sol_found || cost < best_cost) //per come ho creato il codice e la funzione non improving arrivi qui solo se è migliore del best precedente
            {
               if (first_sol_found)
                  cerr << "New best solution " << out <<endl;
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

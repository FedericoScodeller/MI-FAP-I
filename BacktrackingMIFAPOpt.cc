#include "../include/BacktrackingMIFAPOpt.hh"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ostream>
#include <utility>
#include <vector>

BacktrackingMIFAPOpt::BacktrackingMIFAPOpt(const Input& in, unsigned fl)
                     : BacktrackingOpt(in,fl),
                       mat_ch_cost(in.NetworkSize(),std::vector<std::pair<class Cost, int>>(in.TotCh())),
                       vec_ch_index(in.NetworkSize(),-1),
                       vec_tx_order(in.NetworkSize()),
                       satur_memory_vector(in.NetworkSize(),in.MatBlkCh()),
                       nodes_skipped_per_level(in.NetworkSize(),0)

{
  //TELEMETRY
  full_solution_examinated = 0;
}

void BacktrackingMIFAPOpt::FirstOfLevel()
{
   //THIS METHOD WORK ON THE CHANNEL ORDER
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
   //THIS METHOD WORK ON THE CHANNEL ORDER or to be precise keep going along the already establish order
   // se non mi fermo prima del -1 vado fuori memoria, inutile se uso l`uscita anticipata
  if (vec_ch_index[level] < in.TotCh() - 1 ) {
      //qui ho avuto vari bug logici se tolgo prima rischi di avere casi in cui l'UpOne toglie due volte la stessa misura
      // ORDINE OPERATION: togli old ch cost -> assegna il NUOVO CANALE -> Aggiungi il nuovo costo
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
   //THIS "WORK" ON THE ORDER OF TX
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
   // NEXT LEVEL
   ++level;
   // UPDATE SAT
   // needed only if ch was assigned
   if (level > 0)
   {
      int tx_updated = vec_tx_order[level - 1];
      int start, end;
      std::vector<unsigned> update_list = in.AdjTxFrom(tx_updated);

      // NEED TO LOAD MEMORY FIRST
      satur_memory_vector[level] = satur_memory_vector[level - 1];

      for(auto tx: update_list)
      {
         if (out.Ch(tx) == -1  && in.ChSep(tx_updated,tx)) //altrimenti è ignorato a priori nel calcolo del DSatur, il sep è per i valori che hanno solo interferenza non li considero che saturano il ch
         {
            start = (out.Ch(tx_updated) - in.ChSep(tx_updated,tx) + 1 >= 0) ? out.Ch(tx_updated) - in.ChSep(tx_updated,tx) + 1 : 0;
            end = (out.Ch(tx_updated) + in.ChSep(tx_updated,tx) - 1 < in.TotCh()) ? out.Ch(tx_updated) + in.ChSep(tx_updated,tx) - 1 : in.TotCh() - 1;

            for(int ch = start; ch <= end; ch++)
               satur_memory_vector[level][tx][ch] = true;
         }
      }
   }
   int satur, best_satur, tx_best;
   bool first_best_found = false;
   // MAX SAT
   for (size_t tx = 0; tx < in.NetworkSize(); ++tx)
   {
      if (out.Ch(tx) == -1)
      {
         satur = std::count(satur_memory_vector[level][tx].begin(),satur_memory_vector[level][tx].end(),true);
         if (first_best_found)
         {
            if(satur > best_satur || (satur == best_satur && in.Degree(tx) > in.Degree(tx_best)))
            {
               best_satur = satur;
               tx_best = tx;
            }
         }
         else
         {
            best_satur = satur;
            tx_best = tx;
            first_best_found = true;
         }
      }
   }
   assert(first_best_found); //IS IT EVEN USEFULL?
   // NEXT TX
   vec_tx_order[level] = tx_best;
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

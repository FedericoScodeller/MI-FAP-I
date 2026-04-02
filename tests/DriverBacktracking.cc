#include "../include/BacktrackingMIFAPOpt.hh"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
   std::ifstream is(argv[1]);

   json jsonfile;
   is >> jsonfile;


   Input network(jsonfile);
   BacktrackingMIFAPOpt solver(network, network.NetworkSize() - 1);

   unsigned timer_sec = 3* 60;

   if (solver.SearchTimed(timer_sec))
      cout << "Best solution found " << solver.BestSolution() <<   " (" << solver.NumNodes() << " nodes visited)" << endl;
   else
      cout << "No solution exists" << " (" << solver.NumNodes() << " nodes visited)" << endl;

   //PRINT TELEMETRY
   cout << "number of solution fully examined: " << solver.FullSolutionExaminated() << endl;
   cout << "nodes skipped per level: "<< endl;
   cout << "[ ";
   for(unsigned i = 0; i < network.NetworkSize(); ++i)
      cout << solver.NodesSkippedPerLevel(i) << " ";
   cout << "]" << endl;

   cout << "Last position: LV = " << solver.Level() << endl;
   cout << "[ ";
   for(int i = 0; i <= solver.Level(); ++i)
      cout << solver.VecChIndex(i) << " ";
   for(int i = solver.Level()+1; i < static_cast<int>(network.NetworkSize()); ++i)
      cout << "* ";
   cout << "]" << endl;
   return 0;
}

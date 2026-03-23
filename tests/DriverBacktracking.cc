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

   unsigned timer_sec = 10;
   if (solver.SearchTimed(timer_sec))
   {
      cout << "Best solution found " << solver.BestSolution() <<   " (" << solver.NumNodes() << " nodes visited)" << endl;
   }
   else
      cout << "No solution exists" << " (" << solver.NumNodes() << " nodes visited)" << endl;
   return 0;
}

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


  if (solver.Search())
  {
    Output out = solver.BestSolution();
    cout << "Best solution found " << out
         <<   " (" << solver.NumNodes() << " nodes visited)" << endl;
  }
  else
    cout << "No solution exists"
         << " (" << solver.NumNodes() << " nodes visited)" << endl;
  return 0;
}

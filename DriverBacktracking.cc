#include "../include/Backtracking.hh"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
   ifstream is(argv[1]);

   json jsonfile;
   is >> jsonfile;


   Input network(jsonfile);
   Backtracking solver(network);

   bool intime = solver.TimerSearch(60);

   Output out = solver.BestSolution();
   out.TotalCostCheck();
   if (intime)
      cout << "Best absolute solution found\n" << out << endl;
   else
      cout << "Best solution found before timeout\n" << out << endl;

  return 0;
}

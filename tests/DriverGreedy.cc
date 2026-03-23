#include "../include/Greedy.hh"
#include <fstream>
#include <ios>
#include <iostream>
#include <chrono>
#include <ostream>

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[])
{
    std::ifstream is(argv[1]);
    std::ofstream result(argv[2],ios::app);
    int n=stoi(argv[3]);

    json jsonfile;
    is >> jsonfile;


    Input network(jsonfile);
    Greedy solver(network);

    steady_clock::time_point start, stop;
    long duration;
    for(int i = 0; i < n; i++)
    {
       start = steady_clock::now();
       solver.GreedySolver();
       stop = steady_clock::now();
       duration = duration_cast<microseconds>(stop - start).count();

       result << argv[1] << " " << "greedy: " << duration << " us " << solver.Solution().SolutionCost() << " ";

       solver.ResetSolver();

       start = steady_clock::now();
       solver.DegreeSolver();
       stop = steady_clock::now();
       duration = duration_cast<microseconds>(stop - start).count();

       result << "degree: " << duration << " us " << solver.Solution().SolutionCost() << " ";

       solver.ResetSolver();

       start = steady_clock::now();
       solver.DSaturSolver();
       stop = steady_clock::now();
       duration = duration_cast<microseconds>(stop - start).count();

       result << "dsatur: " << duration << " us " << solver.Solution().SolutionCost() << endl;

       solver.ResetSolver();
    }

    is.close();
    result.close();

    return 0;
}

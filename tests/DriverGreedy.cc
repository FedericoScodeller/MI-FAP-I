#include "../include/Greedy.hh"
#include <fstream>
#include <ios>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[])
{
    std::ifstream is(argv[1]);
    std::ofstream result(argv[2],ios::app);
    json jsonfile;
    is >> jsonfile;


    Input network(jsonfile);
    Greedy solver(network);

    auto start = high_resolution_clock::now();
    solver.DegreeSolver();
    Output solution = solver.Solution();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    result << argv[1] << " " << duration.count() << " us " << solution.TotCost() << endl;



    is.close();
    result.close();

    return 0;
}

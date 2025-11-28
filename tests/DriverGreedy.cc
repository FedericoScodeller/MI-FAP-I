#include "../include/Greedy.hh"
#include <fstream>
#include <ios>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[])
{
    // std::ifstream is(argv[1]);
    // std::ofstream result(argv[2],ios::app);
    // json jsonfile;
    // is >> jsonfile;//it's pointless to measure I can't improve it
    // auto start = high_resolution_clock::now();
    // Input network(jsonfile);
    // auto stop = high_resolution_clock::now();

    // auto duration = duration_cast<microseconds>(stop - start);
    // result << network << endl;
    // result << "Input creation " <<duration.count() << " microseconds" << endl << endl;

    // Output solution(network);
    // start = high_resolution_clock::now();
    // GreedyBasic(network,solution);
    // stop = high_resolution_clock::now();
    // // solution.AssignCh(6,10);
    // // solution.AssignCh(7,0);

    // result << "solution in " <<duration.count() << " microseconds" << endl << endl;
    // cout << "solution cost " << solution.TotCost() << endl;
    // cout << "solution cost alt " << solution.TotCostAlt() << endl;

    // result << solution << endl;
    // is.close();
    // result.close();

    return 0;
}

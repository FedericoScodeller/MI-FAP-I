#include "../include/Input.hh"
#include <fstream>
#include <ios>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[])
{
    ifstream is(argv[1]);
    ofstream result(argv[2],ios::app);
    json jsonfile;
    is >> jsonfile;//it's pointless to measure I can't improve it

    auto start = high_resolution_clock::now();
    Input network(jsonfile);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);
    result << argv[1] << " " << network << " " << duration.count() << " us" << endl;

    is.close();
    result.close();

    return 0;
}

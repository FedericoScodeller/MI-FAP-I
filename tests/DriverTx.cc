#include "../include/Transmitter.hh"
#include <iostream>
#include <fstream>


int main()
{
	 std::ifstream is("Tiny.json");
	 nlohmann::json jsonfile;

	 is >> jsonfile;

	 Transmitter tx(jsonfile["cells"][4],TxType::bcch);

	 std::cout << tx <<std::endl;



	return 0;
}

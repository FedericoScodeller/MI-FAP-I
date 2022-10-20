OPTIONS = -Wall -Wfatal-errors -std=c++17 -O3

test.exe: Greedy_test.o network.o transmitter.o relation.o
	g++ -o test.exe Greedy_test.o network.o transmitter.o relation.o

Greedy_test.o: Greedy_test.cpp network.hpp transmitter.hpp relation.hpp json.hpp
	g++ -c $(OPTIONS) Greedy_test.cpp

relation.o: relation.cpp relation.hpp json.hpp
	g++ -c $(OPTIONS) relation.cpp
	
transmitter.o: transmitter.cpp transmitter.hpp relation.hpp json.hpp
	g++ -c $(OPTIONS) transmitter.cpp

network.o: network.cpp network.hpp transmitter.hpp relation.hpp json.hpp
	g++ -c $(OPTIONS) network.cpp

clean:
	rm *.o *.exe
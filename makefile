OPTIONS = -Wall -Wfatal-errors -std=c++17 -O3

test_dsatur.exe: DSatur_test.o network.o transmitter.o 
	g++ -o test_dsatur.exe DSatur_test.o network.o transmitter.o 

test_greedy.exe: Greedy_test.o network.o transmitter.o 
	g++ -o test_greedy.exe Greedy_test.o network.o transmitter.o 

DSatur_test.o: DSatur_test.cpp network.hpp transmitter.hpp json.hpp
	g++ -c $(OPTIONS) DSatur_test.cpp

Greedy_test.o: Greedy_test.cpp network.hpp transmitter.hpp json.hpp
	g++ -c $(OPTIONS) Greedy_test.cpp

transmitter.o: transmitter.cpp transmitter.hpp json.hpp
	g++ -c $(OPTIONS) transmitter.cpp

network.o: network.cpp network.hpp transmitter.hpp json.hpp
	g++ -c $(OPTIONS) network.cpp

clean:
	rm *.o *.exe
OPTIONS = -Wall -std=c++11

test.exe: 
	 g++ -o $(OPTIONS) test.exe DriverTest.cpp network.cpp network.hpp

COMPILER_OPTIONS= -Wall -Wfatal-errors -O3 -std=c++23 #-DDEBUG

all: TestTx TestInput TestInputTime TestCost TestOutput

#TEST
TestOutput: DriverOutput.o Output.o Cost.o Input.o Transmitter.o
	g++ -o TestOutput DriverOutput.o Output.o Cost.o Input.o Transmitter.o

TestCost: DriverCost.o Cost.o
	g++ -o TestCost DriverCost.o Cost.o

TestTx: DriverTx.o Transmitter.o DriverTimedInput.o
	g++ -o TestTx DriverTx.o Transmitter.o

TestInput: DriverInput.o Input.o Transmitter.o
	g++ -o InputTest DriverInput.o Input.o Transmitter.o

TestInputTime: DriverTimedInput.o Input.o Transmitter.o
	g++ -o TimedInputTest DriverTimedInput.o Input.o Transmitter.o

#DRIVER
DriverOutput.o: DriverOutput.cc Output.cc Output.hh
	g++ -c $(COMPILER_OPTIONS) DriverOutput.cc

DriverCost.o: DriverCost.cc Cost.cc Cost.hh
	g++ -c $(COMPILER_OPTIONS) DriverCost.cc

DriverTimedInput.o: DriverTimedInput.cc Input.hh
	g++ -c $(COMPILER_OPTIONS) DriverTimedInput.cc

DriverInput.o: DriverInput.cc Input.hh
	g++ -c $(COMPILER_OPTIONS) DriverInput.cc

DriverTx.o: Transmitter.hh DriverTx.cc
	g++ -c $(COMPILER_OPTIONS) DriverTx.cc

#SOURCE
Transmitter.o: json.hh Transmitter.hh Transmitter.cc
	g++ -c $(COMPILER_OPTIONS) Transmitter.cc

Input.o: Transmitter.hh Input.hh Input.cc
	g++ -c $(COMPILER_OPTIONS) Input.cc

Cost.o: Cost.cc Cost.hh
	g++ -c $(COMPILER_OPTIONS) Cost.cc

Output.o: Cost.hh Input.hh Output.hh Output.cc
	g++ -c $(COMPILER_OPTIONS) Output.cc

clean:
	rm -f *.o
	rm -f Test*

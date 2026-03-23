OPTIONS=-Wall -Wfatal-errors -O3 -std=c++23 #-DDEBUG_BACKTRACKING -DDEBUG

SRC=./src
INCLUDE=./include
LIB=./libs
TEST=./tests

BUILD=./builds
BIN=./bin

BUILD_TEST=./tests/builds
BIN_TEST=./tests/bin

all: $(BIN_TEST)/TestInput $(BIN_TEST)/TestInputTime $(BIN_TEST)/TestCost $(BIN_TEST)/TestOutput $(BIN_TEST)/TestGreedy $(BIN_TEST)/TestBacktracking

#TEST
$(BIN_TEST)/TestBacktracking: $(BUILD_TEST)/DriverBacktracking.o $(BUILD)/BacktrackingMIFAPOpt.o $(BUILD)/Output.o $(BUILD)/Cost.o $(BUILD)/Input.o
	g++ -o $(BIN_TEST)/TestBacktracking $(BUILD_TEST)/DriverBacktracking.o $(BUILD)/BacktrackingMIFAPOpt.o $(BUILD)/Output.o $(BUILD)/Cost.o $(BUILD)/Input.o

$(BIN_TEST)/TestGreedy: $(BUILD_TEST)/DriverGreedy.o $(BUILD)/Greedy.o $(BUILD)/Output.o $(BUILD)/Cost.o $(BUILD)/Input.o
	g++ -o $(BIN_TEST)/TestGreedy $(BUILD_TEST)/DriverGreedy.o $(BUILD)/Greedy.o $(BUILD)/Output.o $(BUILD)/Cost.o $(BUILD)/Input.o

$(BIN_TEST)/TestOutput: $(BUILD_TEST)/DriverOutput.o $(BUILD)/Output.o $(BUILD)/Cost.o $(BUILD)/Input.o
	g++ -o $(BIN_TEST)/TestOutput $(BUILD_TEST)/DriverOutput.o $(BUILD)/Output.o $(BUILD)/Cost.o $(BUILD)/Input.o

$(BIN_TEST)/TestCost: $(BUILD_TEST)/DriverCost.o $(BUILD)/Cost.o
	g++ -o $(BIN_TEST)/TestCost $(BUILD_TEST)/DriverCost.o $(BUILD)/Cost.o

$(BIN_TEST)/TestInputTime: $(BUILD_TEST)/DriverInputTime.o $(BUILD)/Input.o
	g++ -o $(BIN_TEST)/TestInputTime $(BUILD_TEST)/DriverInputTime.o $(BUILD)/Input.o

$(BIN_TEST)/TestInput: $(BUILD_TEST)/DriverInput.o $(BUILD)/Input.o
	g++ -o $(BIN_TEST)/TestInput $(BUILD_TEST)/DriverInput.o $(BUILD)/Input.o

#DRIVER
$(BUILD_TEST)/DriverBacktracking.o: $(INCLUDE)/BacktrackingMIFAPOpt.hh $(TEST)/DriverBacktracking.cc
	g++ -c $(OPTIONS) $(TEST)/DriverBacktracking.cc -o $(BUILD_TEST)/DriverBacktracking.o

$(BUILD_TEST)/DriverGreedy.o: $(INCLUDE)/Greedy.hh $(TEST)/DriverGreedy.cc
	g++ -c $(OPTIONS) $(TEST)/DriverGreedy.cc -o $(BUILD_TEST)/DriverGreedy.o

$(BUILD_TEST)/DriverOutput.o: $(INCLUDE)/Output.hh $(TEST)/DriverOutput.cc
	g++ -c $(OPTIONS) $(TEST)/DriverOutput.cc -o $(BUILD_TEST)/DriverOutput.o

$(BUILD_TEST)/DriverCost.o: $(INCLUDE)/Cost.hh $(TEST)/DriverCost.cc
	g++ -c $(OPTIONS) $(TEST)/DriverCost.cc -o $(BUILD_TEST)/DriverCost.o

$(BUILD_TEST)/DriverInputTime.o: $(INCLUDE)/Input.hh $(TEST)/DriverInputTime.cc
	g++ -c $(OPTIONS) $(TEST)/DriverInputTime.cc -o $(BUILD_TEST)/DriverInputTime.o

$(BUILD_TEST)/DriverInput.o: $(INCLUDE)/Input.hh $(TEST)/DriverInput.cc
	g++ -c $(OPTIONS) $(TEST)/DriverInput.cc -o $(BUILD_TEST)/DriverInput.o


#SOURCE
$(BUILD)/BacktrackingMIFAPOpt.o: $(INCLUDE)/BacktrackingMIFAPOpt.hh $(SRC)/BacktrackingMIFAPOpt.cc $(LIB)/BacktrackingOpt.hh $(INCLUDE)/Input.hh $(INCLUDE)/Output.hh $(INCLUDE)/Cost.hh
	g++ -c $(OPTIONS) $(SRC)/BacktrackingMIFAPOpt.cc -o $(BUILD)/BacktrackingMIFAPOpt.o

$(BUILD)/Greedy.o: $(INCLUDE)/Input.hh $(INCLUDE)/Output.hh $(INCLUDE)/Cost.hh $(INCLUDE)/Greedy.hh $(SRC)/Greedy.cc
	g++ -c $(OPTIONS) $(SRC)/Greedy.cc -o $(BUILD)/Greedy.o

$(BUILD)/Output.o: $(INCLUDE)/Cost.hh $(INCLUDE)/Input.hh $(INCLUDE)/Output.hh $(SRC)/Output.cc
	g++ -c $(OPTIONS) $(SRC)/Output.cc -o $(BUILD)/Output.o

$(BUILD)/Cost.o: $(INCLUDE)/Cost.hh  $(SRC)/Cost.cc
	g++ -c $(OPTIONS) $(SRC)/Cost.cc -o $(BUILD)/Cost.o

$(BUILD)/Input.o: $(LIB)/json.hh $(INCLUDE)/Input.hh $(SRC)/Input.cc
	g++ -c $(OPTIONS) $(SRC)/Input.cc -o $(BUILD)/Input.o

#CLEAN
clean:
	rm -f $(BIN)/* $(BUILD)/*.o $(BIN_TEST)/* $(BUILD_TEST)/*.o
	clear

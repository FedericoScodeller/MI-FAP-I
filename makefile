OPTIONS=-Wall -Wfatal-errors -O3 -std=c++23 #-DDEBUG

SRC=./src
INCLUDE=./include
LIB=./libs
TEST=./tests

BUILD=./builds
BIN=./bin

BUILD_TEST=./tests/builds
BIN_TEST=./tests/bin

all: $(BIN_TEST)/TestTx $(BIN_TEST)/TestInput $(BIN_TEST)/TestInputTime $(BIN_TEST)/TestCost $(BIN_TEST)/TestOutput

#TEST
$(BIN_TEST)/TestOutput: $(BUILD_TEST)/DriverOutput.o $(BUILD)/Output.o $(BUILD)/Cost.o $(BUILD)/Input.o $(BUILD)/Transmitter.o
	g++ -o $(BIN_TEST)/TestOutput $(BUILD_TEST)/DriverOutput.o $(BUILD)/Output.o $(BUILD)/Cost.o $(BUILD)/Input.o $(BUILD)/Transmitter.o

$(BIN_TEST)/TestCost: $(BUILD_TEST)/DriverCost.o $(BUILD)/Cost.o
	g++ -o $(BIN_TEST)/TestCost $(BUILD_TEST)/DriverCost.o $(BUILD)/Cost.o

$(BIN_TEST)/TestInputTime: $(BUILD_TEST)/DriverInputTime.o $(BUILD)/Input.o $(BUILD)/Transmitter.o
	g++ -o $(BIN_TEST)/TestInputTime $(BUILD_TEST)/DriverInputTime.o $(BUILD)/Input.o $(BUILD)/Transmitter.o

$(BIN_TEST)/TestInput: $(BUILD_TEST)/DriverInput.o $(BUILD)/Input.o $(BUILD)/Transmitter.o
	g++ -o $(BIN_TEST)/TestInput $(BUILD_TEST)/DriverInput.o $(BUILD)/Input.o $(BUILD)/Transmitter.o

$(BIN_TEST)/TestTx: $(BUILD_TEST)/DriverTx.o $(BUILD)/Transmitter.o
	g++ -o $(BIN_TEST)/TestTx $(BUILD_TEST)/DriverTx.o $(BUILD)/Transmitter.o

#DRIVER
$(BUILD_TEST)/DriverOutput.o: $(INCLUDE)/Output.hh $(TEST)/DriverOutput.cc
	g++ -c $(OPTIONS) $(TEST)/DriverOutput.cc -o $(BUILD_TEST)/DriverOutput.o

$(BUILD_TEST)/DriverCost.o: $(INCLUDE)/Cost.hh $(TEST)/DriverCost.cc
	g++ -c $(OPTIONS) $(TEST)/DriverCost.cc -o $(BUILD_TEST)/DriverCost.o

$(BUILD_TEST)/DriverInputTime.o: $(INCLUDE)/Input.hh $(TEST)/DriverInputTime.cc
	g++ -c $(OPTIONS) $(TEST)/DriverInputTime.cc -o $(BUILD_TEST)/DriverInputTime.o

$(BUILD_TEST)/DriverInput.o: $(INCLUDE)/Input.hh $(TEST)/DriverInput.cc
	g++ -c $(OPTIONS) $(TEST)/DriverInput.cc -o $(BUILD_TEST)/DriverInput.o

$(BUILD_TEST)/DriverTx.o: $(TEST)/DriverTx.cc $(INCLUDE)/Transmitter.hh
	g++ -c $(OPTIONS) $(TEST)/DriverTx.cc -o $(BUILD_TEST)/DriverTx.o


#SOURCE
$(BUILD)/Output.o: $(INCLUDE)/Cost.hh $(INCLUDE)/Input.hh $(INCLUDE)/Output.hh $(SRC)/Output.cc
	g++ -c $(OPTIONS) $(SRC)/Output.cc -o $(BUILD)/Output.o

$(BUILD)/Cost.o: $(INCLUDE)/Cost.hh  $(SRC)/Cost.cc
	g++ -c $(OPTIONS) $(SRC)/Cost.cc -o $(BUILD)/Cost.o

$(BUILD)/Input.o: $(INCLUDE)/Transmitter.hh $(INCLUDE)/Input.hh $(SRC)/Input.cc
	g++ -c $(OPTIONS) $(SRC)/Input.cc -o $(BUILD)/Input.o

$(BUILD)/Transmitter.o: $(LIB)/json.hh $(INCLUDE)/Transmitter.hh $(SRC)/Transmitter.cc
	g++ -c $(OPTIONS) $(SRC)/Transmitter.cc -o $(BUILD)/Transmitter.o

#CLEAN
clean:
	rm -f $(BIN)/* $(BUILD)/*.o $(BIN_TEST)/* $(BUILD_TEST)/*.o
	clear

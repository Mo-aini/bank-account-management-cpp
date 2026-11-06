CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2 -Iinclude
SRC      := src/Account.cpp src/Bank.cpp src/Transaction.cpp src/Utils.cpp
OBJ      := $(SRC:src/%.cpp=build/%.o)
TARGET   := build/bank

.PHONY: all run test clean

all: $(TARGET)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ) build/main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

build:
	mkdir -p build

run: $(TARGET)
	./$(TARGET) data

test: $(OBJ) | build
	$(CXX) $(CXXFLAGS) tests/test_bank.cpp $(OBJ) -o build/test_bank
	./build/test_bank

clean:
	rm -rf build

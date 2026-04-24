CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
LGFLAGS = -lgtest -lgtest_main -pthread

APPS = task1_1 task1_2 task2 main
TEST_SRC = ../modules/tests.cpp
TEST_BIN = test_runner

all: $(APPS)

%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(TEST_BIN): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LGFLAGS)

test: $(TEST_BIN)
	./$(TEST_BIN)

run_task1_1: task1_1
	./task1_1

run_task1_2: task1_2
	./task1_2

run_task2: task2
	./task2

run_main: main
	./main

clean:
	rm -f $(APPS) $(TEST_BIN)
	clear

.PHONY: all clean test run_task1_1 run_task1_2 run_task2 run_main
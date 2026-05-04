CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
LGFLAGS = -lgtest -lgtest_main -pthread

APPS = task1 task2
TEST_SRC = ../modules/tests.cpp
TEST_BIN = test_runner

all: $(APPS)

%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(TEST_BIN): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LGFLAGS)

test: $(TEST_BIN)
	./$(TEST_BIN)

run_task1: task1
	./task1

run_task2: task2
	./task2

clean:
	rm -f $(APPS) $(TEST_BIN)
	clear

.PHONY: all clean test run_task1 run_task2
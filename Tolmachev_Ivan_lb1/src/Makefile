CXX = g++
CXXFLAGS = -Wall -std=c++20
GTEST_FLAGS = -lgtest -pthread

all: main research test

main: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o main

research: ../modules/research.cpp
	$(CXX) $(CXXFLAGS) ../modules/research.cpp -o research

test: ../modules/tests.cpp
	$(CXX) $(CXXFLAGS) ../modules/tests.cpp $(GTEST_FLAGS) -o test

do_main: main
	./main

do_research: research
	./research && python3 ../modules/graphs.py

do_test: test
	./test

clean:
	rm -f main research test *.csv *.png
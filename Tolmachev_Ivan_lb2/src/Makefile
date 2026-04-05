CXX = g++
CXXFLAGS = -Wall -std=c++20
GTEST_FLAGS = -lgtest -pthread

c_task1: 
	$(CXX) $(CXXFLAGS) task1.cpp -o task

c_task2: 
	$(CXX) $(CXXFLAGS) task2.cpp -o task

c_gen_matrix:
	$(CXX) $(CXXFLAGS) ../modules/matrix_generator.cpp -o matrix

gen_matrix1: c_gen_matrix
	./matrix 0 10 0 0 9 2 matrix.txt

gen_matrix2: c_gen_matrix
	./matrix 1 3 0 0 9 matrix.txt

f_task1: c_task1 gen_matrix1
	./task matrix.txt

f_task2: c_task2 gen_matrix2
	./task matrix.txt

task1: c_task1
	./task

task2: c_task2
	./task

c_tests_task1:
	$(CXX) $(CXXFLAGS) ../modules/tests_task1.cpp $(GTEST_FLAGS) -o tests

c_tests_task2:
	$(CXX) $(CXXFLAGS) ../modules/tests_task2.cpp $(GTEST_FLAGS) -o tests

tests_task1: c_tests_task1
	./tests

tests_task2: c_tests_task2
	./tests

clean:
	rm -f task matrix matrix.txt tests
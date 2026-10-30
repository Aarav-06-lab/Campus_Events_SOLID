CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Iinclude
all: events
events: src/main.cpp include/events.hpp
	$(CXX) $(CXXFLAGS) src/main.cpp -o events
test: tests/tests.cpp include/events.hpp
	$(CXX) $(CXXFLAGS) tests/tests.cpp -o tests_runner
	./tests_runner
clean:
	rm -f events tests_runner
.PHONY: all test clean

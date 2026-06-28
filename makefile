CXX = g++
CXXFLAGS = -std=c++17 -Wall -I Process/include -I Manager/include

# Source files
PROCESS_SRC = Process/src/process.cpp
MANAGER_SRC = Manager/src/manager.cpp
TEST_SRC = Manager/test/test.cpp

# Targets
all: test_manager

test_manager: $(PROCESS_SRC) $(MANAGER_SRC) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f test_manager

.PHONY: all clean

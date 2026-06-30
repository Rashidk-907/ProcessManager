CXX = g++
CXXFLAGS = -std=c++17 -Wall -I Process/include -I Manager/include -I Shell/Include

# Source files
PROCESS_SRC = Process/src/Process.cpp
MANAGER_SRC = Manager/src/Manager.cpp
SHELL_SRC = Shell/src/shell.cpp
TEST_SRC = Shell/test/test_shell.cpp

# Targets
all: Pshell

Pshell: $(PROCESS_SRC) $(MANAGER_SRC) $(SHELL_SRC) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f Pshell

.PHONY: all clean

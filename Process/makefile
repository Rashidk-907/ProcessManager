CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra

test: test/test.cpp src/ProcessResource.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f test

run: test
	./test

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -I/opt/homebrew/include -I/usr/local/include -I/usr/include -I/usr/include/gdal
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib
LIBS = -lgdal -ltiff

OBJ_DIR = build
SRC_DIR = src
TEST_DIR = tests
TEST_TARGET = run_tests

# Source files for tests (only SearchAlgorithm and dijkstras)
TEST_SOURCES := $(SRC_DIR)/search_algorithms/SearchAlgorithm.cpp $(SRC_DIR)/search_algorithms/dijkstras.cpp $(TEST_DIR)/tests.cpp

TEST_OBJECTS := $(OBJ_DIR)/search_algorithms/SearchAlgorithm.o $(OBJ_DIR)/search_algorithms/dijkstras.o $(OBJ_DIR)/tests.o

# Build object files for SearchAlgorithm
$(OBJ_DIR)/search_algorithms/SearchAlgorithm.o: $(SRC_DIR)/search_algorithms/SearchAlgorithm.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build object files for dijkstras
$(OBJ_DIR)/search_algorithms/dijkstras.o: $(SRC_DIR)/search_algorithms/dijkstras.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build object file for the test file
$(OBJ_DIR)/tests.o: $(TEST_DIR)/tests.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build the test executable
$(TEST_TARGET): $(TEST_OBJECTS)
	$(CXX) $(TEST_OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Run the tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TEST_TARGET)

.PHONY: clean test
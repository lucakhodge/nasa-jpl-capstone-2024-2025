CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
# Include Google Test headers from your tests directory
INCLUDES = -I/opt/homebrew/include -I/usr/local/include -I/usr/include -I/usr/include/gdal -Itests/googletest/googletest/include
# Point to locally built Google Test libraries
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib -L/usr/lib/x86_64-linux-gnu -Ltests/googletest/build/lib
LIBS = -lgdal -ltiff
TEST_LIBS = -lgdal -ltiff -lgtest -lgtest_main -pthread

TARGET = simulator
TEST_TARGET = run_tests

OBJ_DIR = build
SRC_DIR = src
TEST_DIR = tests

SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
TEST_SOURCES := $(shell find $(TEST_DIR) -type f -name '*_test.cpp')

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_SOURCES))

# Simulator target
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Test dependencies (adjust based on your source files)
TEST_DEP_OBJECTS := $(OBJ_DIR)/search_algorithms/dijkstras.o $(OBJ_DIR)/search_algorithms/SearchAlgorithm.o

# Test target
$(TEST_TARGET): $(TEST_DEP_OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $(TEST_DEP_OBJECTS) $(TEST_OBJECTS) -o $@ $(LDFLAGS) $(TEST_LIBS)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile test files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET) tests/googletest/build

test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: clean test
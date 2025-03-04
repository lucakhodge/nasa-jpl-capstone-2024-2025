CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -I/opt/homebrew/include -I/usr/local/include -I/usr/include
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib
LIBS = -lgdal -ltiff

TARGET = simulator
OBJ_DIR = build
SRC_DIR = src
TEST_SRC_DIR = tests
GTEST_DIR = tests/googletest/googletest

# Simulator sources and objects
SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
SIMULATOR_OBJ = $(OBJ_DIR)/rover_standin/simulator.o
IMPL_OBJECTS = $(filter-out $(SIMULATOR_OBJ), $(OBJECTS))

# Test sources and objects
TEST_SOURCES := $(wildcard $(TEST_SRC_DIR)/*.cpp)
TEST_OBJECTS := $(patsubst $(TEST_SRC_DIR)/%.cpp, $(OBJ_DIR)/tests/%.o, $(TEST_SOURCES))
GTEST_OBJ = $(OBJ_DIR)/gtest-all.o
TEST_TARGET = run_tests

# Default target: build the simulator
all: $(TARGET)

# Build the simulator
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Compile simulator source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I$(SRC_DIR) -c $< -o $@

# Compile GTest
$(GTEST_OBJ): $(GTEST_DIR)/src/gtest-all.cc
	$(CXX) $(CXXFLAGS) -I$(GTEST_DIR)/include -I$(GTEST_DIR) -c $< -o $@

# Compile test source files
$(OBJ_DIR)/tests/%.o: $(TEST_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I$(SRC_DIR) -I$(GTEST_DIR)/include -c $< -o $@

# Build the test executable
$(TEST_TARGET): $(IMPL_OBJECTS) $(TEST_OBJECTS) $(GTEST_OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS) -lpthread

# Run the tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)

.PHONY: all test clean
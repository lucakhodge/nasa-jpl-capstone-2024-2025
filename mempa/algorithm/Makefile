CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -I/opt/homebrew/include -I/usr/local/include -I/usr/include -I/usr/include/gdal
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib
LIBS = -lgdal -ltiff

TARGET = simulator

OBJ_DIR = build
SRC_DIR = src
TEST_DIR = tests
TEST_TARGET = run_tests

# Source files for the main program (all .cpp files in src)
SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')

# Source files for tests (only SearchAlgorithm and dijkstras)
TEST_SOURCES := $(SRC_DIR)/SearchAlgorithm.cpp $(SRC_DIR)/dijkstras.cpp $(TEST_DIR)/tests.cpp

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
TEST_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(filter $(SRC_DIR)/SearchAlgorithm.cpp $(SRC_DIR)/dijkstras.cpp, $(TEST_SOURCES)))
TEST_OBJECTS += $(OBJ_DIR)/tests.o

# Build the main simulator target
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Build object files for the main program
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
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
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)

.PHONY: clean test
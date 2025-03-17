CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -I/opt/homebrew/include -I/usr/local/include -I/usr/include -I/usr/include/gdal
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib
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

# Simulator dependencies (all objects)
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Test dependencies
TEST_DEP_OBJECTS := $(OBJ_DIR)/search_algorithms/dijkstras.o $(OBJ_DIR)/search_algorithms/SearchAlgorithm.o

# Debug: Print variables
print-vars:
	@echo "TEST_SOURCES = $(TEST_SOURCES)"
	@echo "TEST_OBJECTS = $(TEST_OBJECTS)"
	@echo "TEST_DEP_OBJECTS = $(TEST_DEP_OBJECTS)"

$(TEST_TARGET): $(TEST_DEP_OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(TEST_DEP_OBJECTS) $(TEST_OBJECTS) -o $@ $(LDFLAGS) $(TEST_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: clean test print-vars
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

SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
# OBJECTS = $(SOURCES:SRC_DIR/%.cpp=$(OBJ_DIR)/%.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

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
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: clean


# If you are getting a segmentation fault or a vtable error while working with the search algorithm strategy pattern implemntation try resaving every cpp file assosicated with it.
# Currently dijkstras.cpp, SearchAlgorithm.cpp, SearchContext.cpp.  
# I don't know exactly why this is but because of the pointer/memory stuff associated with the strategy pattern in c++ all of these files must be properly linked and if there ->
# were no modifications to these files they may not be properly linked.  If anyone has a way to make sure all of these files are recomiped when one is changed I think it will fix this issue.  


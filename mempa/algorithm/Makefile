CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -Isrc -I/opt/homebrew/include -I/usr/local/include -I/usr/include -I/usr/include/gdal
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib
LIBS = -lgdal -ltiff

OBJ_DIR = build
SRC_DIR = src
TEST_DIR = tests
SEARCH_TEST_TARGET = run_search_tests
DEM_TEST_TARGET = run_dem_tests

# Source files for search tests (SearchAlgorithm, dijkstras, and tests.cpp)
SEARCH_TEST_SOURCES := $(SRC_DIR)/search_algorithms/SearchAlgorithm.cpp \
                       $(SRC_DIR)/search_algorithms/dijkstras.cpp \
                       $(TEST_DIR)/tests.cpp

SEARCH_TEST_OBJECTS := $(OBJ_DIR)/search_algorithms/SearchAlgorithm.o \
                       $(OBJ_DIR)/search_algorithms/dijkstras.o \
                       $(OBJ_DIR)/tests.o

# Source files for DEM tests (DemHandler and DemTester.cpp)
DEM_TEST_SOURCES := $(SRC_DIR)/DemHandler/DemHandler.cpp \
                    $(SRC_DIR)/DemHandler/DemTester.cpp

DEM_TEST_OBJECTS := $(OBJ_DIR)/DemHandler/DemHandler.o \
                    $(OBJ_DIR)/DemHandler/DemTester.o

# Build object files for SearchAlgorithm
$(OBJ_DIR)/search_algorithms/SearchAlgorithm.o: $(SRC_DIR)/search_algorithms/SearchAlgorithm.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build object files for dijkstras
$(OBJ_DIR)/search_algorithms/dijkstras.o: $(SRC_DIR)/search_algorithms/dijkstras.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build object file for the search test file (tests.cpp)
$(OBJ_DIR)/tests.o: $(TEST_DIR)/tests.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build object files for DemHandler
$(OBJ_DIR)/DemHandler/DemHandler.o: $(SRC_DIR)/DemHandler/DemHandler.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build object files for DemTester
$(OBJ_DIR)/DemHandler/DemTester.o: $(SRC_DIR)/DemHandler/DemTester.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build the search test executable (for tests.cpp)
$(SEARCH_TEST_TARGET): $(SEARCH_TEST_OBJECTS)
	$(CXX) $(SEARCH_TEST_OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Build the DEM test executable (for DemTester.cpp)
$(DEM_TEST_TARGET): $(DEM_TEST_OBJECTS)
	$(CXX) $(DEM_TEST_OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Run both test executables
test: $(SEARCH_TEST_TARGET) $(DEM_TEST_TARGET)
	./$(SEARCH_TEST_TARGET)
	./$(DEM_TEST_TARGET)

clean:
	rm -rf $(OBJ_DIR) $(SEARCH_TEST_TARGET) $(DEM_TEST_TARGET)

.PHONY: clean test
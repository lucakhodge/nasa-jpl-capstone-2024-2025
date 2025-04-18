# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -Isrc -I/opt/homebrew/include -I/usr/local/include -I/usr/include \
           -I/usr/include/gdal -I/usr/local/include/gdal -Iexternal/json/include \
           -I/home/linuxbrew/.linuxbrew/include
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib
LIBS = -lgdal -ltiff

# Directories and targets
TARGET = simulator.out
OBJ_DIR = build
SRC_DIR = src
TEST_DIR = tests
SEARCH_TEST_TARGET = run_search_tests.out
DEM_TEST_TARGET = run_dem_tests.out

# Main program sources and objects
SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Source files for search tests
SEARCH_TEST_SOURCES := $(SRC_DIR)/dem-handler/DemHandler.cpp \
                       $(SRC_DIR)/rover-simulator/RoverSimulator.cpp \
                       $(SRC_DIR)/search_algorithms/SearchAlgorithm.cpp \
                       $(TEST_DIR)/DijkstrasTester.cpp

SEARCH_TEST_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SEARCH_TEST_SOURCES))

# Source files for DEM tests
DEM_TEST_SOURCES := $(SRC_DIR)/dem-handler/DemHandler.cpp \
                    $(SRC_DIR)/rover-simulator/RoverSimulator.cpp \
                    $(SRC_DIR)/rover-pathfinding-module/SearchAlgorithm.cpp \
                    $(TEST_DIR)/DemTester.cpp

DEM_TEST_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(DEM_TEST_SOURCES))

# Build target
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Generic object file rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Add specific rule if needed for DemHandler
$(OBJ_DIR)/dem-handler/DemHandler.o: $(SRC_DIR)/dem-handler/DemHandler.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Test targets
$(SEARCH_TEST_TARGET): $(SEARCH_TEST_OBJECTS)
	$(CXX) $(SEARCH_TEST_OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

$(DEM_TEST_TARGET): $(DEM_TEST_OBJECTS)
	$(CXX) $(DEM_TEST_OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Test running
test: $(SEARCH_TEST_TARGET) $(DEM_TEST_TARGET)
	./$(SEARCH_TEST_TARGET)
	./$(DEM_TEST_TARGET) tests/mars_dem.tif 5

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(SEARCH_TEST_TARGET) $(DEM_TEST_TARGET)

.PHONY: clean test
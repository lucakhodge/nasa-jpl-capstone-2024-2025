# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Detect OS
ifeq ($(OS),Windows_NT)
    CONDA_PREFIX ?= $(shell echo $$CONDA_PREFIX)
    CXXFLAGS += -Ideps/json -I$(CONDA_PREFIX)/Library/include -I$(CONDA_PREFIX)/Library/include/gdal
    LDFLAGS += -L$(CONDA_PREFIX)/Library/lib
else
    CXXFLAGS += -Ideps/json
    INCLUDES = -Isrc -I/usr/include -I/usr/include/gdal -I/usr/local/include -I/usr/local/include/gdal -I/opt/homebrew/include
    LDFLAGS += -L/usr/lib -L/usr/local/lib -L/opt/homebrew/lib
endif

LIBS = -lgdal -ltiff

# Project structure
TARGET = simulator.out
OBJ_DIR = build
SRC_DIR = src
TEST_DIR = tests

# Test executables
SEARCH_TEST_TARGET = run_search_tests.out
DEM_TEST_TARGET = run_dem_tests.out

# Source/object files
SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

SEARCH_TEST_SOURCES := $(SRC_DIR)/DemHandler/DemHandler.cpp \
                       $(SRC_DIR)/rover-simulator/RoverSimulator.cpp \
                       $(SRC_DIR)/search_algorithms/SearchAlgorithm.cpp \
                       $(SRC_DIR)/search_algorithms/dijkstras.cpp \
                       $(TEST_DIR)/DijkstrasTester.cpp

SEARCH_TEST_OBJECTS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(subst $(SRC_DIR)/,,$(SEARCH_TEST_SOURCES)))

DEM_TEST_SOURCES := $(SRC_DIR)/DemHandler/DemHandler.cpp \
                    $(SRC_DIR)/rover-simulator/RoverSimulator.cpp \
                    $(SRC_DIR)/rover-pathfinding-module/SearchAlgorithm.cpp \
                    $(SRC_DIR)/rover-pathfinding-module/dijkstras.cpp \
                    $(TEST_DIR)/DemTester.cpp

DEM_TEST_OBJECTS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(subst $(SRC_DIR)/,,$(DEM_TEST_SOURCES)))

# Targets
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Generic object rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Test executables
$(SEARCH_TEST_TARGET): $(SEARCH_TEST_OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)

$(DEM_TEST_TARGET): $(DEM_TEST_OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)

# Run tests
test: $(SEARCH_TEST_TARGET) $(DEM_TEST_TARGET)
	./$(SEARCH_TEST_TARGET)
	./$(DEM_TEST_TARGET) tests/mars_dem.tif 5

# Clean build
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(SEARCH_TEST_TARGET) $(DEM_TEST_TARGET)

.PHONY: clean test

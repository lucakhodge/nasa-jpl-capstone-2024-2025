# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Dynamic include paths that work across environments
GDAL_CFLAGS := $(shell gdal-config --cflags 2>/dev/null || echo "-I/usr/include/gdal -I/usr/local/include/gdal -I/opt/homebrew/include/gdal")
GDAL_LIBS := $(shell gdal-config --libs 2>/dev/null || echo "-lgdal")

INCLUDES = -Isrc $(GDAL_CFLAGS) -Iexternal/json/include
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib
LIBS = $(GDAL_LIBS) -ltiff

# Directories and targets
TARGET = simulator.out
OBJ_DIR = build
SRC_DIR = src
TEST_DIR = tests
SEARCH_TEST_TARGET = run_search_tests.out
DEM_TEST_TARGET = run_dem_tests.out

# Create build directories if they don't exist
$(shell mkdir -p $(OBJ_DIR) $(OBJ_DIR)/dem-handler $(OBJ_DIR)/rover-simulator $(OBJ_DIR)/rover-pathfinding-module $(OBJ_DIR)/tests)

# Main program sources and objects
SOURCES := $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Source files for search tests
SEARCH_TEST_SOURCES := $(SRC_DIR)/dem-handler/DemHandler.cpp \
                      $(SRC_DIR)/rover-simulator/RoverSimulator.cpp \
                      $(SRC_DIR)/rover-pathfinding-module/SearchAlgorithm.cpp \
                      $(TEST_DIR)/SearchAlgorithmTester.cpp

SEARCH_TEST_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SEARCH_TEST_SOURCES))
SEARCH_TEST_OBJECTS += $(OBJ_DIR)/tests/SearchAlgorithmTester.o

# Source files for DEM tests
DEM_TEST_SOURCES := $(SRC_DIR)/dem-handler/DemHandler.cpp \
                   $(SRC_DIR)/rover-simulator/RoverSimulator.cpp \
                   $(SRC_DIR)/rover-pathfinding-module/SearchAlgorithm.cpp \
                   $(TEST_DIR)/DemTester.cpp

DEM_TEST_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(DEM_TEST_SOURCES))
DEM_TEST_OBJECTS += $(OBJ_DIR)/tests/DemTester.o

# Build target
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Generic object file rule for source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Rule for test files
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Test targets
$(SEARCH_TEST_TARGET): $(SEARCH_TEST_OBJECTS)
	$(CXX) $(SEARCH_TEST_OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

$(DEM_TEST_TARGET): $(DEM_TEST_OBJECTS)
	$(CXX) $(DEM_TEST_OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Test running - check if test files exist before running
test: $(SEARCH_TEST_TARGET) $(DEM_TEST_TARGET)
	@if [ -f "./$(SEARCH_TEST_TARGET)" ]; then ./$(SEARCH_TEST_TARGET); fi
	@if [ -f "./$(DEM_TEST_TARGET)" ] && [ -f "$(TEST_DIR)/mars_dem.tif" ]; then \
		./$(DEM_TEST_TARGET) $(TEST_DIR)/mars_dem.tif 5; \
	else \
		echo "Missing DEM test file or mars_dem.tif"; \
		exit 1; \
	fi

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(SEARCH_TEST_TARGET) $(DEM_TEST_TARGET)

.PHONY: clean test
# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Find GDAL include paths - try multiple approaches for better cross-platform compatibility
GDAL_CONFIG := $(shell which gdal-config 2>/dev/null)

ifeq ($(GDAL_CONFIG),)
  # If gdal-config is not found, use hardcoded paths
  GDAL_CFLAGS = -I/usr/include/gdal -I/usr/local/include/gdal -I/opt/homebrew/include/gdal
  GDAL_LIBS = -L/usr/lib -L/usr/lib/x86_64-linux-gnu -L/usr/local/lib -L/opt/homebrew/lib -lgdal
else
  # Use gdal-config if available
  GDAL_CFLAGS = $(shell gdal-config --cflags)
  GDAL_LIBS = $(shell gdal-config --libs)
endif

# Additional include and library paths for robustness
INCLUDES = -Isrc -I/opt/homebrew/include -I/usr/local/include -I/usr/include -I/usr/include/gdal -I/usr/local/include/gdal -L/usr/lib -lgdal -Iexternal/json/include -I/home/linuxbrew/.linuxbrew/include
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib -L/usr/lib/x86_64-linux-gnu
LIBS = $(GDAL_LIBS) -ltiff

# Directories and targets
TARGET = simulator.out
OBJ_DIR = build
SRC_DIR = src
TEST_DIR = tests
DEM_TEST_TARGET = run_dem_tests.out

# Create build directories if they don't exist
$(shell mkdir -p $(OBJ_DIR)/dem-handler $(OBJ_DIR)/rover-simulator $(OBJ_DIR)/rover-pathfinding-module $(OBJ_DIR)/tests)

# Main program sources and objects
SOURCES := $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Source files for DEM tests (do not include the test cpp file directly)
DEM_TEST_SOURCES := $(SRC_DIR)/dem-handler/DemHandler.cpp \
                   $(SRC_DIR)/rover-simulator/RoverSimulator.cpp \
                   $(SRC_DIR)/rover-pathfinding-module/SearchAlgorithm.cpp \
                   $(SRC_DIR)/rover-pathfinding-module/NewDijkstras.cpp

# Convert source files to object files
DEM_TEST_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(DEM_TEST_SOURCES))
# Add the test object file
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

# Test targets - add INCLUDES to the linker command to help find headers
$(DEM_TEST_TARGET): $(DEM_TEST_OBJECTS)
	$(CXX) $(DEM_TEST_OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Test running - check if test files exist before running
test: $(DEM_TEST_TARGET)
	@if [ -f "./$(DEM_TEST_TARGET)" ] && [ -f "$(TEST_DIR)/mars_dem.tif" ]; then \
		./$(DEM_TEST_TARGET) $(TEST_DIR)/mars_dem.tif 5; \
	elif [ -f "./$(DEM_TEST_TARGET)" ]; then \
		echo "Warning: mars_dem.tif not found, running test without it"; \
		./$(DEM_TEST_TARGET); \
	else \
		echo "Error: DEM test executable not built"; \
		exit 1; \
	fi

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(DEM_TEST_TARGET)

.PHONY: clean test

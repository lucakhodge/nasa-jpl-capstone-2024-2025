CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -Isrc -I/opt/homebrew/include -I/usr/local/include -I/usr/include -I/usr/include/gdal -I/usr/local/include/gdal -L/usr/lib -lgdal -Iexternal/json/include -I/home/linuxbrew/.linuxbrew/include
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib
LIBS = -lgdal -ltiff

TARGET = simulator.out
OBJ_DIR = build
SRC_DIR = src
TEST_DIR = tests
SEARCH_TEST_TARGET = run_search_tests.out
DEM_TEST_TARGET = run_dem_tests.out

# Main program sources and objects
SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Source files for search tests (SearchAlgorithm, dijkstras, and DijkstrasTester.cpp)
SEARCH_TEST_SOURCES := $(SRC_DIR)/DemHandler/DemHandler.cpp \
					   $(SRC_DIR)/rover-simulator/RoverSimulator.cpp \
					   $(SRC_DIR)/search_algorithms/SearchAlgorithm.cpp \

SEARCH_TEST_OBJECTS := $(OBJ_DIR)/DemHandler/DemHandler.o \
					   $(OBJ_DIR)/rover-simulator/RoverSimulator.o \
					   $(OBJ_DIR)/search_algorithms/SearchAlgorithm.o \
                       $(OBJ_DIR)/search_algorithms/dijkstras.o \
                       $(OBJ_DIR)/tests.o

# Source files for DEM tests (DemHandler and DemTester.cpp)
DEM_TEST_SOURCES := $(SRC_DIR)/DemHandler/DemHandler.cpp \
					$(SRC_DIR)/rover-simulator/RoverSimulator.cpp \
					$(SRC_DIR)/rover-pathfinding-module/SearchAlgorithm.cpp \
					$(SRC_DIR)/rover-pathfinding-module/dijkstras.cpp \
                    $(TEST_DIR)/DemTester.cpp

DEM_TEST_OBJECTS := $(OBJ_DIR)/DemHandler/DemHandler.o \
					$(OBJ_DIR)/rover-simulator/RoverSimulator.o \
					$(OBJ_DIR)/rover-pathfinding-module/SearchAlgorithm.o \
					$(OBJ_DIR)/search_algorithms/dijkstras.o \
                    $(OBJ_DIR)/tests/DemTester.o

# Main target
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

# Generic object file rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Specific object file rules
$(OBJ_DIR)/search_algorithms/SearchAlgorithm.o: $(SRC_DIR)/rover-pathfinding-module/SearchAlgorithm.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/search_algorithms/dijkstras.o: $(SRC_DIR)/rover-pathfinding-module/dijkstras.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/tests.o: $(TEST_DIR)/DijkstrasTester.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/DemHandler/DemHandler.o: $(SRC_DIR)/dem-handler/DemHandler.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/tests/DemTester.o: $(TEST_DIR)/DemTester.cpp
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

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(SEARCH_TEST_TARGET) $(DEM_TEST_TARGET)

.PHONY: clean test

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
    LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib
    LDFLAGS += -L/usr/lib -L/usr/local/lib -L/opt/homebrew/lib
endif

LIBS = -lgdal -ltiff

OBJ_DIR = obj
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
TARGET = program

SEARCH_TEST_SRC = $(wildcard tests/search_*.cpp)
SEARCH_TEST_OBJ = $(patsubst tests/%.cpp,$(OBJ_DIR)/%.o,$(SEARCH_TEST_SRC))
SEARCH_TEST_TARGET = search_test

DEM_TEST_SRC = tests/dem_test.cpp
DEM_TEST_OBJ = $(patsubst tests/%.cpp,$(OBJ_DIR)/%.o,$(DEM_TEST_SRC))
DEM_TEST_TARGET = dem_test

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)

$(SEARCH_TEST_TARGET): $(SEARCH_TEST_OBJ)
	$(CXX) $(SEARCH_TEST_OBJ) -o $@ $(LDFLAGS) $(LIBS)

$(DEM_TEST_TARGET): $(DEM_TEST_OBJ)
	$(CXX) $(DEM_TEST_OBJ) -o $@ $(LDFLAGS) $(LIBS)

# Test running
test: $(SEARCH_TEST_TARGET) $(DEM_TEST_TARGET)
	./$(SEARCH_TEST_TARGET)
	./$(DEM_TEST_TARGET) tests/mars_dem.tif 5

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(SEARCH_TEST_TARGET) $(DEM_TEST_TARGET)

.PHONY: clean test
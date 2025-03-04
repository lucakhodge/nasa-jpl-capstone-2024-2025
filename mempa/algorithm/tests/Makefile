CC = g++
CFLAGS = -Wall -g -std=c++17
GTEST_DIR = ./googletest
GTEST_INC = $(GTEST_DIR)/googletest/include
GTEST_LIB_DIR = $(GTEST_DIR)/build/lib
SRC_DIR = ../src
OBJ_DIR = obj
BIN_DIR = bin

# Main app
APP_SOURCES = $(wildcard $(SRC_DIR)/**/*.cpp)
APP_OBJECTS = $(APP_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
APP_EXEC = $(BIN_DIR)/search_algorithms

# Test files
TEST_SOURCES = $(wildcard *.cpp)
TEST_OBJECTS = $(TEST_SOURCES:%.cpp=$(OBJ_DIR)/%.o)
# Source files for testing (exclude main.cpp)
SRC_SOURCES = $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))
SRC_OBJECTS = $(SRC_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TEST_EXEC = $(BIN_DIR)/run_tests

# Default target
all: build run

# Build the main program, clone GTest if missing
build: $(GTEST_DIR) $(APP_EXEC)

# Clone googletest if not present
$(GTEST_DIR):
	@echo "Cloning googletest..."
	git clone https://github.com/google/googletest.git $(GTEST_DIR)

# Build googletest if not built
$(GTEST_LIB_DIR): $(GTEST_DIR)
	@echo "Building googletest..."
	cd $(GTEST_DIR) && mkdir -p build && cd build && cmake .. && make

# Run the tests, ensuring GTest is built
run: $(GTEST_LIB_DIR) $(TEST_EXEC)
	./$(TEST_EXEC)

# Link main app
$(APP_EXEC): $(APP_OBJECTS) | $(BIN_DIR)
	@if [ -z "$(APP_OBJECTS)" ]; then echo "Error: No object files for main app!"; exit 1; fi
	$(CC) $(APP_OBJECTS) -o $@

# Link test executable
$(TEST_EXEC): $(TEST_OBJECTS) $(SRC_OBJECTS) | $(BIN_DIR)
	@if [ -z "$(TEST_OBJECTS)" ] || [ -z "$(SRC_OBJECTS)" ]; then echo "Error: No test or source object files!"; exit 1; fi
	$(CC) $(TEST_OBJECTS) $(SRC_OBJECTS) -L$(GTEST_LIB_DIR) -lgtest -lgtest_main -pthread -o $@

# Compile app source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test source files
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(GTEST_INC) -I$(SRC_DIR) -c $< -o $@

# Create directories
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

CFLAGS += -I/usr/include/gdal

# Debugging: Print variable values
print-vars:
	@echo "APP_SOURCES: $(APP_SOURCES)"
	@echo "APP_OBJECTS: $(APP_OBJECTS)"
	@echo "SRC_SOURCES: $(SRC_SOURCES)"
	@echo "SRC_OBJECTS: $(SRC_OBJECTS)"
	@echo "TEST_SOURCES: $(TEST_SOURCES)"
	@echo "TEST_OBJECTS: $(TEST_OBJECTS)"

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all build run clean print-vars

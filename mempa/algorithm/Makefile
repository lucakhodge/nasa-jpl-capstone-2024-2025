CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -I/opt/homebrew/include -I/usr/local/include -I/usr/include
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib
LIBS = -lgdal -ltiff

TARGET = simulator

OBJ_DIR = build
SRC_DIR = src

SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
# OBJECTS = $(SOURCES:SRC_DIR/%.cpp=$(OBJ_DIR)/%.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: clean

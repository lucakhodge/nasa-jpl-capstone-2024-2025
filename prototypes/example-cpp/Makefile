# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Source file
SRC = example.cpp

TARGET = example

$(TARGET):
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Clean build files
clean:
	rm -f $(TARGET)

.PHONY: all clean darwin-universal

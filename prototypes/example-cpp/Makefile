# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Source file
SRC = example.cpp

TARGET = example

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -I/opt/homebrew/include $(SRC) -o $(TARGET)

# Clean build files
clean:
	rm -f $(TARGET)

.PHONY: all clean darwin-universal

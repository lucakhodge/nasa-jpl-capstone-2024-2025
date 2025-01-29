# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Source file
SRC = example.cpp

# Platform-specific output names
WINDOWS_OUT = example-win.exe
DARWIN_X64_OUT = example-darwin-x64
DARWIN_ARM64_OUT = example-darwin-arm64
LINUX_OUT = example-linux

# Default target (adjust based on your platform)
all: $(WINDOWS_OUT) darwin-universal $(LINUX_OUT)

# Windows build (requires MinGW or similar)
$(WINDOWS_OUT):
	$(CXX) $(CXXFLAGS) -target x86_64-pc-windows-gnu $(SRC) -o $(WINDOWS_OUT)

# macOS x86_64 (Intel) build
$(DARWIN_X64_OUT):
	$(CXX) $(CXXFLAGS) -target x86_64-apple-darwin $(SRC) -o $(DARWIN_X64_OUT)

# macOS arm64 (Apple Silicon) build
$(DARWIN_ARM64_OUT):
	$(CXX) $(CXXFLAGS) -target arm64-apple-darwin $(SRC) -o $(DARWIN_ARM64_OUT)

# Create universal binary for macOS (combines x64 and arm64)
darwin-universal: $(DARWIN_X64_OUT) $(DARWIN_ARM64_OUT)
	lipo -create -output example-darwin $(DARWIN_X64_OUT) $(DARWIN_ARM64_OUT)

# Linux build
$(LINUX_OUT):
	$(CXX) $(CXXFLAGS) -target x86_64-linux-gnu $(SRC) -o $(LINUX_OUT)

# Clean build files
clean:
	rm -f $(WINDOWS_OUT) $(DARWIN_X64_OUT) $(DARWIN_ARM64_OUT) example-darwin $(LINUX_OUT)

.PHONY: all clean darwin-universal
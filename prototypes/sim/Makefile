CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -I/usr/local/include
LDFLAGS = -L/usr/local/lib
LIBS = -lgdal -ltiff

OBJDIR = build

SOURCES = simulator.cpp \
    ../search_algorithms/dijkstras.cpp \
    ../../algorithm/src/mempa_dem_handler/mempa_dem/buf_dem.cpp

OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

simulator: $(OBJECTS)
    @mkdir -p $(dir $@)
    $(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

$(OBJDIR)/%.o: %.cpp
    @mkdir -p $(dir $@)
    $(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
    rm -rf $(OBJDIR) simulator

.PHONY: clean

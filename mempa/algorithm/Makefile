CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -I/opt/homebrew/include -I/usr/local/include -I/usr/include
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -L/usr/lib
LIBS = -lgdal -ltiff

OBJDIR = build

OBJECTS = build/dijkstras.o build/buff_dem.o build/simulator.o

simulator: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

build/simulator.o: simulator.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o build/simulator.o

build/dijkstras.o: ../search_algorithms/dijkstras.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o build/dijkstras.o

build/buff_dem.o: ../../algorithm/src/mempa_dem_handler/mempa_dem/buf_dem.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o build/buff_dem.o

clean:
	rm -rf $(OBJDIR) simulator

.PHONY: clean

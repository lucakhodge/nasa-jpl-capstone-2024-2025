# Mars Rover Path Planning Simulator

## Building the Simulator

### Prerequisites
- g++ compiler
- GDAL library
- TIFF library

### Compilation
```bash
# Using make (recommended)
make clean && make

# Manual compilation
g++ -std=c++17 -o simulator simulator.cpp /file_path/dijkstras.cpp /file_path/buf_dem.cpp -I/usr/local/include -L/usr/local/lib -lgdal -ltiff

example: 
g++ -std=c++17 -o simulator simulator.cpp /Users/adamcarlson/Dropbox/Mac/Desktop/Desktop/nasa-jpl-capstone-2024-2025/prototypes/search_algorithms/dijkstras.cpp /Users/adamcarlson/Dropbox/Mac/Desktop/Desktop/nasa-jpl-capstone-2024-2025/algorithm/src/mempa_dem_handler/mempa_dem/buf_dem.cpp -I/usr/local/include -L/usr/local/lib -lgdal -ltiff
```
2. Run the code using the following command:

```
Use the following command to run the simulator with default coordinates:
./simulator

Use the following command to run the simulator with specific coordinates:
./simulator <start_x> <start_y> <end_x> <end_y>
example:
./simulator 1254 1265 1340 1338
```

### Running Multiple Tests
```
# First build the simulator then:
cd /sim/tests
./run_tests.sh
# Output will be saved in /sim/test_results
```

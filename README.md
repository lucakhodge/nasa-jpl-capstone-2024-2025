# NASA JPL Capstone 2024/2025

<div align="center" >
  <img src="https://pasadena-library.net/teens/wp-content/blogs.dir/18/files/sites/18/2017/12/jpl.gif" alt="nasa jpl logo" height="400">
  <br/>
Mars Elevation Model Pathfinding Algorithm (MEMPA)
  <br/>
  <a href="https://github.com/lucakhodge/nasa-jpl-capstone-2024-2025/tree/main/mempa">Source Code</a> | <a href="https://github.com/lucakhodge/nasa-jpl-capstone-2024-2025/tree/main/docs">Documentation</a>
  <br/>
  <br/>
</div>

## Team Members

- Adam Carlson, Brock Hoos, Grace Harrel, Kevin Buhler, Luca Hodge, Oscar Mikus, Ryan Wagster
- David Blaines (TA),  Marcel Llopis (Sponsor)

## Dependencies

- Windows Subsystem for Linux (WSL)
- GDAL headers: ```sudo apt install libgdal-dev```
- tiff file: https://astrogeology.usgs.gov/search/map/mars_mgs_mola_mex_hrsc_blended_dem_global_200m

## Folder Structure
```
.
├── C++.gitignore
├── README.md
├── assets
│   └── NASAJPL_Logo.png
├── docs
│   ├── html
│   └── latex
├── mempa
│   ├── algorithm
│   │   ├── Doxyfile
│   │   ├── Makefile
│   │   ├── README.md
│   │   ├── build
│   │   │   └── search_algorithms
│   │   │       ├── SearchAlgorithm.o
│   │   │       └── dijkstras.o
│   │   ├── src
│   │   │   ├── DemHandler
│   │   │   │   ├── DemHandler.cpp
│   │   │   │   ├── DemHandler.h
│   │   │   │   └── DemTester.cpp
│   │   │   ├── buf_main
│   │   │   │   └── buf_main.cpp
│   │   │   ├── cli-draft
│   │   │   │   ├── cli.h
│   │   │   │   ├── cli_draft.cpp
│   │   │   │   ├── cli_main.cpp
│   │   │   │   ├── test.dem
│   │   │   │   └── test.txt
│   │   │   ├── rover_standin
│   │   │   │   ├── RoverSimulator.cpp
│   │   │   │   ├── RoverSimulator.h
│   │   │   │   └── simulator.cpp
│   │   │   └── search_algorithms
│   │   │       ├── SearchAlgorithm.cpp
│   │   │       ├── SearchAlgorithm.h
│   │   │       ├── SearchContext.cpp
│   │   │       ├── SearchContext.h
│   │   │       ├── dijkstras.cpp
│   │   │       └── dijkstras.h
│   │   ├── test_results
│   │   └── tests
│   │       ├── run_tests.sh
│   │       ├── test_coordinates.json
│   │       └── tests.cpp
│   ├── gui
│   └── mars-pathfinding-gui
└── prototypes
```
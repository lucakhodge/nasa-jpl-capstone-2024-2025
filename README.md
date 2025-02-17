# NASA JPL Capstone 2024/2025

<div align="center" >
  <img src="https://pasadena-library.net/teens/wp-content/blogs.dir/18/files/sites/18/2017/12/jpl.gif" alt="nasa jpl logo" height="400">
  <br/>
Mars Elevation Model Pathfinding Algorithm (MEMPA)
  <br/>
  <a href="">source code</a> | <a href="">documentation</a>
  <br/>
  <br/>
</div>

## Team Members

  Adam Carlson, Brock Hoos, Grace Harrel, Kevin Buhler, Luca Hodge, Oscar Mikus, Ryan Wagster
  
  David Blaines (TA),  Marcel Llopis (Sponsor)

## How to Run (still in progress)

### CLI:
To compile: ```g++ ./cli_draft.cpp -o cli```

To run: ```./cli --flag INPUT```

Example: ```./cli --start 44.43,55.55 --end 32.22,55.33 --input test.dem --output test.txt --iterations 4 --slope 12.3```

### DEM: ?
### SIM: ?
### ELECTRON: ?
### map2d: ?
### mars-pathfinding: ?
### search_algorithms: ?

## File Structure

```
root/
├── assets/
├── docs/                          # how to see?
│   ├── docs/
│   └── latex/
├── prototypes/
│   ├── cli-draft/                 
│   ├── example-cpp/               # necessary?
│   ├── first_dem_cpp/             # undocumented
│   │   ├── mempa_algo/
│   │   ├── mempa_dem/
│   │   ├── buf_main.cpp
│   │   ├── C++.gitignore
│   │   └── dem_preprocessor.py
│   ├── luca-electron/             # how to run?
│   │   ├── cpp-program/
│   │   ├── my-app/
│   │   └── my-vite-app/
│   ├── map2d/                     # necessary?
│   │   └── my-new-app/
│   ├── mars-pathfinding/          # necessary?
│   ├── search_algorithms/         # how to run?
│   │   ├── dijkstra.h
│   │   └── dijkstra.cpp
│   └── sim/                       # undocumented
│       └── simulator.cpp
└── src/                        
    └── README.md
```

## Dependencies

- Windows Subsystem for Linux (WSL)
- GDAL headers: ```sudo apt install libgdal-dev```
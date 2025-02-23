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

## Installation

Command: (still in progress)

```bash
make run
``` 

## Folder Structure
```
.
├── .vscode/
├── assets/
├── docs/
├── mempa/
│   ├── algorithm/
│   │   └── src/
│   │       ├── mempa_dem_handler/
│   │       ├── search_algorithms/
│   │       ├── .gitignore
│   │       ├── Doxyfile
│   │       └── Makefile
│   ├── gui/
│   └── README.md
├── prototypes/
│   ├── .gitignore
│   ├── C++.gitignore
│   └── README.md
```

## Dependencies

- Windows Subsystem for Linux (WSL)
- GDAL headers: ```sudo apt install libgdal-dev```
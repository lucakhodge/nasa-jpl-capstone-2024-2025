cmake_minimum_required(VERSION 3.10)
project(MarsRoverPathfinding)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(GDAL REQUIRED)
find_package(TIFF REQUIRED)

# Add source files
add_executable(simulator
    src/rover_standin/simulator.cpp
    src/search_algorithms/dijkstras.cpp
    src/dem_handler/buf_dem.cpp
)

# Add include directories
target_include_directories(simulator PRIVATE
    src/rover_standin
    src/search_algorithms
    src/dem_handler
    ${GDAL_INCLUDE_DIRS}
    ${TIFF_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(simulator PRIVATE 
    ${GDAL_LIBRARIES}
    ${TIFF_LIBRARIES}
)

# Add compiler flags
if(APPLE)
    target_compile_options(simulator PRIVATE -Wall -Wextra)
endif()
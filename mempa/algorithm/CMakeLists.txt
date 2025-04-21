cmake_minimum_required (VERSION 3.16)
project (mars_pathfinding CXX)

# ───────── compiler options ─────────
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS  OFF)

# ───────── dependencies ─────────────
find_package (GDAL           REQUIRED)
find_package (TIFF           REQUIRED)
find_package (nlohmann_json  REQUIRED)

# Header search paths not auto‑propagated
include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/src     # for “rover-pathfinding-module/…”
    ${GDAL_INCLUDE_DIRS}                # for <gdal_priv.h>
)

# ───────── library sources ──────────
file (GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
      src/dem-handler/*.cpp
      src/logger/*.cpp
      src/metrics/*.cpp
      src/rover-pathfinding-module/*.cpp
      src/rover-simulator/*.cpp)

add_library (pathfinding STATIC ${ENGINE_SOURCES})
target_link_libraries (pathfinding
    PRIVATE GDAL::GDAL TIFF::TIFF nlohmann_json::nlohmann_json)

# ───────── CLI executable ───────────
add_executable (mars_pathfinder
    src/main/main.cpp
    src/main/CLI.cpp)
target_link_libraries (mars_pathfinder PRIVATE pathfinding)

# ───────── unit tests ───────────────
include (CTest)          # gives the built‑in “test” target
enable_testing()

add_executable (DemTester        tests/DemTester.cpp)
add_executable (DijkstrasTester  tests/DijkstrasTester.cpp)
target_link_libraries (DemTester        PRIVATE pathfinding)
target_link_libraries (DijkstrasTester  PRIVATE pathfinding)

# ───────── test args ───────────────
set (DEM_FILE ${CMAKE_CURRENT_SOURCE_DIR}/tests/mars_dem.tif)
set (DEM_CHUNK_SIZE 50)

add_test (NAME DemTester
          COMMAND DemTester ${DEM_FILE} ${DEM_CHUNK_SIZE})
add_test (NAME DijkstrasTester
          COMMAND DijkstrasTester)

set_tests_properties (DemTester DijkstrasTester
    PROPERTIES ENVIRONMENT "PRINT_OUTPUT=$ENV{PRINT_OUTPUT}")

# alias mimicking “make test”
add_custom_target (check
    DEPENDS  DemTester DijkstrasTester
    COMMAND  ${CMAKE_CTEST_COMMAND} --output-on-failure)

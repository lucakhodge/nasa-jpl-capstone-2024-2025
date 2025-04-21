cmake_minimum_required (VERSION 3.16)
project (mars_pathfinding CXX)

# ────────────────────── compiler options ───────────────────────
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS  OFF)

# ────────────────────── dependencies ───────────────────────────
find_package (GDAL           REQUIRED)          # libgdal-dev
find_package (TIFF           REQUIRED)          # libtiff5-dev
find_package (nlohmann_json  REQUIRED)

# 1️⃣  Header search paths that aren’t automatically propagated
include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/src     # <gives “rover-pathfinding-module/…”>
    ${GDAL_INCLUDE_DIRS}                # <gives <gdal_priv.h>>
)

# ────────────────────── library code ───────────────────────────
file (GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
      src/dem-handler/*.cpp
      src/logger/*.cpp
      src/metrics/*.cpp
      src/rover-pathfinding-module/*.cpp
      src/rover-simulator/*.cpp)

add_library (pathfinding STATIC ${ENGINE_SOURCES})
target_link_libraries (pathfinding
    PRIVATE
        GDAL::GDAL
        TIFF::TIFF
        nlohmann_json::nlohmann_json)

# ────────────────────── CLI executable ─────────────────────────
add_executable (mars_pathfinder
    src/main/main.cpp
    src/main/CLI.cpp)

target_link_libraries (mars_pathfinder PRIVATE pathfinding)

# ────────────────────── unit tests ─────────────────────────────
include (CTest)     # supplies the built‑in “test” target
enable_testing()

add_executable (DemTester        tests/DemTester.cpp)
add_executable (DijkstrasTester  tests/DijkstrasTester.cpp)

foreach (t DemTester DijkstrasTester)
    target_link_libraries (${t} PRIVATE pathfinding)
    add_test (NAME ${t} COMMAND ${t})
endforeach()

# honour existing env‑var used by the legacy Makefile
set_tests_properties (DemTester DijkstrasTester
    PROPERTIES ENVIRONMENT "PRINT_OUTPUT=$ENV{PRINT_OUTPUT}")

# Handy alias mimicking the old “make test”
add_custom_target (check
    DEPENDS  DemTester DijkstrasTester
    COMMAND  ${CMAKE_CTEST_COMMAND} --output-on-failure)

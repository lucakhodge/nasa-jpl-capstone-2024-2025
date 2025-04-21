cmake_minimum_required (VERSION 3.16)
project (mars_pathfinding CXX)

# ──────────────────  compiler options  ──────────────────
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS  OFF)

# ──────────────────  dependencies  ──────────────────────
find_package (GDAL           REQUIRED)          # libgdal-dev
find_package (TIFF           REQUIRED)          # libtiff5-dev
find_package (nlohmann_json  REQUIRED)

# Some FindGDAL versions don’t propagate include‑dirs, so add them manually
include_directories (${GDAL_INCLUDE_DIRS})

# ──────────────────  engine / library  ──────────────────
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

# ──────────────────  CLI executable  ────────────────────
add_executable (mars_pathfinder
    src/main/main.cpp
    src/main/CLI.cpp)

target_link_libraries (mars_pathfinder PRIVATE pathfinding)

# ──────────────────  unit tests  ────────────────────────
include (CTest)         # supplies the built‑in “test” target
enable_testing()

add_executable (DemTester        tests/DemTester.cpp)
add_executable (DijkstrasTester  tests/DijkstrasTester.cpp)

foreach (t DemTester DijkstrasTester)
    target_link_libraries (${t} PRIVATE pathfinding)
    add_test (NAME ${t} COMMAND ${t})
endforeach()

# Pass through the old PRINT_OUTPUT env‑var
set_tests_properties (DemTester DijkstrasTester
    PROPERTIES ENVIRONMENT "PRINT_OUTPUT=$ENV{PRINT_OUTPUT}")

# Optional convenience alias:  cmake --build . --target check
add_custom_target (check
    DEPENDS  DemTester DijkstrasTester        # ensures they’re built
    COMMAND  ${CMAKE_CTEST_COMMAND} --output-on-failure)

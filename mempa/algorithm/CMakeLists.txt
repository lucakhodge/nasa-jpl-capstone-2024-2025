cmake_minimum_required(VERSION 3.16)
project(mars_pathfinding LANGUAGES CXX)

# ─────── Compiler settings ───────
set(CMAKE_CXX_STANDARD        20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS       OFF)

# ─────── Dependencies ───────────
# Try config‐mode first (picks up vcpkg on Windows), then module‐mode on Linux
find_package(GDAL          REQUIRED)   # libgdal-dev / vcpkg gdal
find_package(TIFF          REQUIRED)   # libtiff5-dev / vcpkg tiff
find_package(nlohmann_json REQUIRED)   # nlohmann-json3-dev / vcpkg nlohmann-json

# Some FindGDAL versions set GDAL_INCLUDE_DIRS but don’t propagate them
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src    # your “rover-pathfinding-module/…” headers
  ${GDAL_INCLUDE_DIRS}               # legacy var from FindGDAL
)

# ─────── Library code ────────────
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
     src/dem-handler/*.cpp
     src/logger/*.cpp
     src/metrics/*.cpp
     src/rover-pathfinding-module/*.cpp
     src/rover-simulator/*.cpp)

add_library(pathfinding STATIC ${ENGINE_SOURCES})
target_link_libraries(pathfinding
  PRIVATE
    GDAL::GDAL
    TIFF::TIFF
    nlohmann_json::nlohmann_json
)

# ─────── CLI executable ──────────
add_executable(mars_pathfinder
  src/main/main.cpp
  src/main/CLI.cpp
)
target_link_libraries(mars_pathfinder PRIVATE pathfinding)

# ─────── Unit tests ──────────────
include(CTest)       # defines the built‑in “test” target
enable_testing()

add_executable(DemTester       tests/DemTester.cpp)
add_executable(DijkstrasTester tests/DijkstrasTester.cpp)

foreach(t IN ITEMS DemTester DijkstrasTester)
  target_link_libraries(${t} PRIVATE pathfinding)
endforeach()

# Pass in the DEM file and chunk size you download in CI
set(DEM_FILE       ${CMAKE_CURRENT_SOURCE_DIR}/tests/mars_dem.tif)
set(DEM_CHUNK_SIZE 50)

add_test(NAME DemTester       COMMAND DemTester       ${DEM_FILE} ${DEM_CHUNK_SIZE})
add_test(NAME DijkstrasTester COMMAND DijkstrasTester)

set_tests_properties(DemTester DijkstrasTester
  PROPERTIES ENVIRONMENT "PRINT_OUTPUT=$ENV{PRINT_OUTPUT}"
)

# “make check” / “cmake --build . --target check”
add_custom_target(check
  DEPENDS DemTester DijkstrasTester
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
)

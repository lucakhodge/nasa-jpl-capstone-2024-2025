cmake_minimum_required(VERSION 3.16)
project(mars_pathfinding LANGUAGES CXX)

# ────── Compiler settings ────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Windows‑specific niceties
if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX)
endif()

# ────── Dependencies ────────────────────────────────────────────────────
find_package(GDAL          REQUIRED)
find_package(TIFF          REQUIRED)
find_package(nlohmann_json REQUIRED)

# ────── Library code ────────────────────────────────────────────────────
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
     src/dem-handler/*.cpp
     src/logger/*.cpp
     src/metrics/*.cpp
     src/rover-pathfinding-module/*.cpp
     src/rover-simulator/*.cpp)

add_library(pathfinding STATIC ${ENGINE_SOURCES})

target_include_directories(pathfinding
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(pathfinding
  PUBLIC
    GDAL::GDAL
    TIFF::TIFF
    nlohmann_json::nlohmann_json
)

# ────── CLI executable ──────────────────────────────────────────────────
add_executable(mars_pathfinder
  src/main/main.cpp
  src/main/CLI.cpp
)
target_link_libraries(mars_pathfinder PRIVATE pathfinding)

# ────── Unit tests ──────────────────────────────────────────────────────
include(CTest)
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

add_custom_target(check
  DEPENDS DemTester DijkstrasTester
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
)

# mempa/algorithm/CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(mempa_algorithm VERSION 1.0 LANGUAGES CXX)

# Compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# keep your warning flags and defines
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -D_USE_MATH_DEFINES")

# Find external dependencies
find_package(GDAL REQUIRED)
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${GDAL_INCLUDE_DIRS}
)

# Main executable: all .cpp under src/
file(GLOB_RECURSE MAIN_SRC
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)
add_executable(simulator ${MAIN_SRC})
target_link_libraries(simulator
  PRIVATE
    ${GDAL_LIBRARIES}
    nlohmann_json::nlohmann_json
)

# --- Tests ---------------------------------------------------

# Search tests
set(SEARCH_TEST_SOURCES
  ${PROJECT_SOURCE_DIR}/src/DemHandler/DemHandler.cpp
  ${PROJECT_SOURCE_DIR}/src/rover-simulator/RoverSimulator.cpp
  ${PROJECT_SOURCE_DIR}/src/search_algorithms/SearchAlgorithm.cpp
  ${PROJECT_SOURCE_DIR}/src/search_algorithms/dijkstras.cpp
  ${PROJECT_SOURCE_DIR}/tests/DijkstrasTester.cpp
)
add_executable(run_search_tests ${SEARCH_TEST_SOURCES})
target_include_directories(run_search_tests PRIVATE ${PROJECT_SOURCE_DIR}/src ${GDAL_INCLUDE_DIRS})
target_link_libraries(run_search_tests PRIVATE ${GDAL_LIBRARIES} nlohmann_json::nlohmann_json)

# DEM tests
set(DEM_TEST_SOURCES
  ${PROJECT_SOURCE_DIR}/src/DemHandler/DemHandler.cpp
  ${PROJECT_SOURCE_DIR}/src/rover-simulator/RoverSimulator.cpp
  ${PROJECT_SOURCE_DIR}/src/rover-pathfinding-module/SearchAlgorithm.cpp
  ${PROJECT_SOURCE_DIR}/src/rover-pathfinding-module/dijkstras.cpp
  ${PROJECT_SOURCE_DIR}/tests/DemTester.cpp
)
add_executable(run_dem_tests ${DEM_TEST_SOURCES})
target_include_directories(run_dem_tests PRIVATE ${PROJECT_SOURCE_DIR}/src ${GDAL_INCLUDE_DIRS})
target_link_libraries(run_dem_tests PRIVATE ${GDAL_LIBRARIES} nlohmann_json::nlohmann_json)

enable_testing()
add_test(NAME SearchTests COMMAND run_search_tests)
add_test(NAME DemTests COMMAND run_dem_tests ../tests/mars_dem.tif 5)

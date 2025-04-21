cmake_minimum_required(VERSION 3.16)
project(mars_pathfinding CXX)

# ─── compiler settings ───────────────────────────────────────────────
set(CMAKE_CXX_STANDARD        20)
set(CMAKE_CXX_EXTENSIONS      OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── find your deps ─────────────────────────────────────────────────
find_package(GDAL           REQUIRED)   # libgdal-dev
find_package(TIFF           REQUIRED)   # libtiff5-dev
find_package(nlohmann_json  REQUIRED)   # nlohmann-json3-dev

# expose GDAL headers to every target (so <gdal_priv.h> is found)
include_directories(${GDAL_INCLUDE_DIRS})

# ─── project includes & sources ─────────────────────────────────────
include_directories(src)

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

# ─── main CLI executable ────────────────────────────────────────────
add_executable(mars_pathfinder
    src/main/main.cpp
    src/main/CLI.cpp
)
target_link_libraries(mars_pathfinder PRIVATE pathfinding)

# ─── tests & “check” target ─────────────────────────────────────────
include(CTest)
enable_testing()

# unit‐test executables
add_executable(DemTester       tests/DemTester.cpp)
add_executable(DijkstrasTester tests/DijkstrasTester.cpp)

foreach(t DemTester DijkstrasTester)
    target_link_libraries(${t} PRIVATE pathfinding)
    add_test(NAME ${t} COMMAND ${t})
    set_tests_properties(${t}
        PROPERTIES
          ENVIRONMENT "PRINT_OUTPUT=$ENV{PRINT_OUTPUT}"
    )
endforeach()

# “check” builds the tests then runs them
add_custom_target(check
    DEPENDS  DemTester DijkstrasTester
    COMMAND  ${CMAKE_CTEST_COMMAND} --output-on-failure
)

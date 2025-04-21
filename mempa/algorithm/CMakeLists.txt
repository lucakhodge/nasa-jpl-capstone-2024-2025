cmake_minimum_required(VERSION 3.10)
project(MEMPA_Algorithm LANGUAGES CXX)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find external dependencies
find_package(GDAL REQUIRED)
find_package(nlohmann_json REQUIRED)

# Define source directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_SOURCE_DIR}/tests)

# Gather all .cpp source files (excluding the CLI main)
file(GLOB_RECURSE CORE_SOURCES
    "${SRC_DIR}/*.cpp"
)
list(REMOVE_ITEM CORE_SOURCES "${SRC_DIR}/main/main.cpp")

# Create a library target for core algorithm code
add_library(mempa_core ${CORE_SOURCES})
target_include_directories(mempa_core PUBLIC ${SRC_DIR})
target_link_libraries(mempa_core PUBLIC GDAL::GDAL nlohmann_json::nlohmann_json)

# Create the CLI executable
add_executable(mempa_algorithm "${SRC_DIR}/main/main.cpp")
target_link_libraries(mempa_algorithm PRIVATE mempa_core)

# Tests
enable_testing()
file(GLOB TEST_SOURCES "${TEST_DIR}/*.cpp")
foreach(test_src IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE mempa_core)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()


cmake_minimum_required(VERSION 3.10)
project(MEMPA_Algorithm LANGUAGES CXX)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find external dependencies
find_package(GDAL REQUIRED)
find_package(nlohmann_json REQUIRED)

# Where to find your headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# Collect all your library sources
file(GLOB_RECURSE ALGO_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# Main executable
add_executable(mempa_algorithm
    ${ALGO_SOURCES}
)
target_link_libraries(mempa_algorithm
    PRIVATE GDAL::GDAL
            nlohmann_json::nlohmann_json
)

# Tests
enable_testing()
file(GLOB TEST_SOURCES
    "${CMAKE_SOURCE_DIR}/tests/*.cpp"
)
foreach(test_src IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name}
        ${test_src}
        ${ALGO_SOURCES}
    )
    target_link_libraries(${test_name}
        PRIVATE GDAL::GDAL
                nlohmann_json::nlohmann_json
    )
    add_test(NAME ${test_name}
             COMMAND ${test_name}
    )
endforeach()

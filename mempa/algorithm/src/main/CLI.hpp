#pragma once

/* mempa::DemHandler */
#include "../dem-handler/DemHandler.hpp"

/* C++ Standard Libraries */
#include <limits>
#include <string>
#include <vector>
#include <utility>

/* POSIX Libraries */
#include <getopt.h>

namespace mempa
{
    /**
     * @brief Command Line Interface for the MEMPA Project.
     *
     * ## Instructions for Use
     *
     * 1. Make sure you are in the correct directory where the CLI file is located.
     *
     * 2. To compile:
     *    @code
     *    `g++ ./main.cpp -o cli`
     *    @endcode
     *
     * 3. To run:
     *    @code
     *    `./cli --flag INPUT`
     *    @endcode
     *
     *    Example:
     *    @code
     *    `./cli --start 44.43,55.55 --end 32.22,55.33 --input test.dem --output test.txt --memory 256 --slope 12.3 --radius 3.4`
     *    @endcode
     *
     * ## Required Flags
     *
     * - Two of the following (one must be a strStartCoordinate-type and one an strGoalCoordinate-type):
     *   - `--start`
     *   - `--end`
     *   - `--start-pixel`
     *   - `--end-pixel`
     *   - `--start_area`
     *   - `--end_area`
     *
     * - `--input`          (Path to the input DEM file)
     * - `--output`         (Path to the output file)
     * - `--memory`         (Amount of memory capacity on rover in kilobytes)
     * - `--slope`          (Max slope threshold)
     * - `--radius`         (Search radius)
     *
     * @note Use `--help` to print out detailed input formats for each flag.
     *
     * @author Brock Hoos <brock.hoos@colorado.edu>
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    class CLI
    {
    private:
        /* Array for CLI user arguments. */
        inline static constexpr struct option longOptions[] = {
            {"start", required_argument, nullptr, 's'},
            {"end", required_argument, nullptr, 'e'},
            {"start-area", required_argument, nullptr, 'a'},
            {"end-area", required_argument, nullptr, 'b'},
            {"start-pixel", required_argument, nullptr, 'x'},
            {"end-pixel", required_argument, nullptr, 'y'},
            {"input", required_argument, nullptr, 'i'},
            {"output", required_argument, nullptr, 'o'},
            {"memory", required_argument, nullptr, 'm'},
            {"slope", required_argument, nullptr, 'p'},
            {"radius", required_argument, nullptr, 'r'},
            {"json", no_argument, nullptr, 'j'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, 0, nullptr, 0}};
        inline static constexpr const char *shortOptions = "s:e:a:b:i:o:m:p:h"; /* Single character identifiers for getopt_long(). */

        float maxSlopeTolerance = std::numeric_limits<float>::quiet_NaN(); /* User defined maximum slope tolerance (0° - 90°) */
        int pixelBuffer = std::numeric_limits<int>::quiet_NaN();           /* Radius or buffer to be passed to RoverSimulator and then DemHandler. */

        int memorySize = std::numeric_limits<int>::quiet_NaN(); /* Memory constraint. */

        bool makeJSON = false; /* Flag to set whether to use a text or json output file format. */

        bool isStartSet = false; /* Tracks if the starting position has been set. */
        bool isGoalSet = false;  /* Tracks if the goal position has been set. */

        bool imgCRS = false; /* Tracks if the inputs are using <x, y> coordinates. */
        bool geoCRS = false; /* Tracks if the inputs are using <x, y> coordinates. */

        std::pair<int, int> imgStartPosition{std::numeric_limits<int>::quiet_NaN(), std::numeric_limits<int>::quiet_NaN()}; /* Starting image based (x, y) coordinates. */
        std::pair<int, int> imgGoalPosition{std::numeric_limits<int>::quiet_NaN(), std::numeric_limits<int>::quiet_NaN()};  /* Image based (x, y) coordinate goal. */

        std::pair<double, double> geoStartPosition{std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()}; /* Starting geospatial (lat, long) coordinates. */
        std::pair<double, double> geoGoalPosition{std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()};  /* Geospatial (lat, long) coordinate goal. */

        std::string geoStrStartCoordinate; /* User input start (lat, long) coordinate. */
        std::string geoStrGoalCoordinate;  /* User input goal (lat, long) coordinate. */

        std::string geoStrStartArea; /* User input start (lat, long) region. */
        std::string geoStrGoalArea;  /* User input goal (lat, long) region. */

        std::string imgStrStartCoordinate; /* User input start (x, y) coordinate. */
        std::string imgStrGoalCoordinate;  /* User input goal (x, y) coordinate. */

        char *geotiffFilepath;      /* User input full path to DEM geotiff file. */
        std::string outputFilename; /* User input filename for output file. */

        std::pair<double, double> parseGeoCoordinates(std::string inputCoordinate) const;
        std::pair<int, int> parsePixelCoordinates(std::string inputCoordinate) const;
        std::pair<std::pair<double, double>, std::pair<double, double>> parseGeoArea(std::string inputRegion) const;

        template <typename ordinate1, typename ordinate2>
        inline bool isCoordinateSet(std::pair<ordinate1, ordinate2> coordinatePair) const noexcept;
        inline std::pair<double, double> getRandomPointInGeoArea(std::pair<double, double> coordinate1, std::pair<double, double> coordinate2) const noexcept;
        inline bool isStartCoordinateSet() noexcept;
        inline bool isGoalCoordinateSet() noexcept;
        inline void print_helper() const noexcept;

    protected:
        /* CLI is not designed to be subclassed. */

    public:
        explicit CLI(int argc, char *const argv[]);
        inline void displayInputs() const noexcept;
        inline char *getGeotiffFilepath() const noexcept;
        inline std::string getOutputFilename() const noexcept;
        inline bool isImgCRS() const noexcept;
        inline bool isGeoCRS() const noexcept;
        inline bool getJSONFlag() const noexcept;
        inline float getSlopeTolerance() const noexcept;
        inline int getMemorySize() const noexcept;
        inline int getBufferSize() const noexcept;
        inline std::pair<std::pair<double, double>, std::pair<double, double>> getGeoCoordinates() const noexcept;
        inline std::pair<std::pair<int, int>, std::pair<int, int>> getImgCoordinates() const noexcept;
    };
}

#include "CLI.inl"

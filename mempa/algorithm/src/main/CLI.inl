/* Local Header */
#include "CLI.hpp"

/* C++ Standard Libraries */
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <utility>
#include <stdexcept>

namespace mempa
{
    /**
     * @brief Checks to see if a coordinate pair of any type has been initialized.
     *
     * @tparam ordinate1 Type of first ordinate.
     * @tparam ordinate2 Type of second ordinate.
     * @param coordinatePair Coordinate pair to check.
     * @return true Coordinate pair has been initialized.
     * @return false Coordinate pair has not been initialized.
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    template <typename ordinate1, typename ordinate2>
    inline bool CLI::isCoordinateSet(const std::pair<ordinate1, ordinate2> coordinatePair) const noexcept
    {
        return !(std::isnan(coordinatePair.first) || std::isnan(coordinatePair.second));
    }

    /**
     * @brief For a (lat, long) area created by parseGeoArea, get a random point from that region.
     *
     * @param coordinate1 First coordinate defining the region.
     * @param coordinate2 Second coordinate defining the region.
     * @return std::pair<double, double> Random (lat, long) coordinate from the region.
     *
     * @author Brock Hoos <brock.hoos@colorado.edu>
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline std::pair<double, double> CLI::getRandomPointInGeoArea(const std::pair<double, double> coordinate1, const std::pair<double, double> coordinate2) const noexcept
    {
        const double randomLongitude = coordinate1.first + static_cast<double>(rand() / RAND_MAX) * (coordinate2.first - coordinate1.first);
        const double randomLatitude = coordinate1.second + static_cast<double>(rand() / RAND_MAX) * (coordinate2.second - coordinate1.second);
        return std::pair<double, double>(randomLongitude, randomLatitude);
    }

    /**
     * @brief Tracks whether or not the intial position has been set.
     *
     * @return true
     * @return false
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline bool CLI::isStartCoordinateSet() noexcept
    {
        return isStartSet ? true : (isStartSet = true, false);
    }

    /**
     * @brief Tracks whether or not the goal coordinate has been set.
     *
     * @return true
     * @return false
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline bool CLI::isGoalCoordinateSet() noexcept
    {
        return isGoalSet ? true : (isGoalSet = true, false);
    }

    /**
     * @brief Print out helper of flags for each type of input.
     *
     * @author Brock Hoos <brock.hoos@colorado.edu>
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline void CLI::print_helper() const noexcept
    {
        std::cout << R"(
            Options:
              --start          Start coordinates (e.g., <double>,<double> or 'random')
              --end            End coordinates (e.g., <double>,<double>)
              --start-area     Start area range (e.g., <double>,<double>:<double>,<double>)
              --end-area       End area range (e.g., <double>,<double>:<double>,<double>)
              --start-pixel    Start pixel coordinate (e.g., <int>,<int>)
              --end-pixel      End pixel coordinate (e.g., <int>,<int>)
              --input          Input DEM file (.tif format)
              --output         Output results file
              --iterations     Number of iterations
              --slope          Slope tolerances (e.g., 10,20,30)
              --radius         Visibility Radius of Rover (in meters)
              --json           Print output into JSON format
              --help           Print help message
            )" << std::endl;
    }

    /**
     * @brief Display inputs received and initialized from user.
     *
     * @author Brock Hoos <brock.hoos@colorado.edu>
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline void CLI::displayInputs() const noexcept
    {
        std::cout << "Start: " << geoStrStartCoordinate
                  << "\nEnd: " << geoStrGoalCoordinate
                  << "\nStart Area: " << geoStrStartArea
                  << "\nEnd Area: " << geoStrGoalArea
                  << "\nStart (Pixel): " << imgStrStartCoordinate
                  << "\nEnd (Pixel): " << imgStrGoalCoordinate
                  << "\nInput File: " << geotiffFilepath
                  << "\nOutput File: " << outputFilename
                  << "\nIterations: " << numIterations
                  << "\nSlope Tolerance: " << maxSlopeTolerance
                  << "\nRadius: " << pixelBuffer
                  << std::endl;
    }

    /**
     * @brief Return the DEM filepath.
     *
     * @return char*
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline char *CLI::getGeotiffFilepath() const noexcept
    {
        return geotiffFilepath;
    }

    /**
     * @brief Get the output filename for logging.
     *
     * @return std::string
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline std::string CLI::getOutputFilename() const noexcept
    {
        return outputFilename;
    }

    /**
     * @brief Return if the input is (x, y) coordinates.
     *
     * @return true
     * @return false
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline bool CLI::isImgCRS() const noexcept
    {
        return imgCRS;
    }

    /**
     * @brief Return if the input is (lat, long) coordinate.
     *
     * @return true
     * @return false
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline bool CLI::isGeoCRS() const noexcept
    {
        return geoCRS;
    }

    /**
     * @brief Get the JSON creation flag.
     *
     * @return true
     * @return false
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline bool CLI::getJSONFlag() const noexcept
    {
        return makeJSON;
    }

    /**
     * @brief Get the max slope tolerance.
     *
     * @return float
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline float CLI::getSlopeTolerance() const noexcept
    {
        return maxSlopeTolerance;
    }

    /**
     * @brief Get the desired pixel buffering area.
     *
     * @return int
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline int CLI::getBufferSize() const noexcept
    {
        return pixelBuffer;
    }

    /**
     * @brief Return both geospatial coordinates.
     *
     * @return std::pair<std::pair<double, double>, std::pair<double, double>>
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline std::pair<std::pair<double, double>, std::pair<double, double>> CLI::getGeoCoordinates() const noexcept
    {
        return std::pair<std::pair<double, double>, std::pair<double, double>>(geoStartPosition, geoGoalPosition);
    }

    /**
     * @brief Return both image based coordinates.
     *
     * @return std::pair<std::pair<int, int>, std::pair<int, int>>
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    inline std::pair<std::pair<int, int>, std::pair<int, int>> CLI::getImgCoordinates() const noexcept
    {
        return std::pair<std::pair<int, int>, std::pair<int, int>>(imgStartPosition, imgGoalPosition);
    }
}
/* Local Header */
#include "CLI.hpp"

/* C++ Standard Libraries */
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <utility>
#include <stdexcept>

/* POSIX Libraries */
#include <getopt.h>

namespace mempa
{
    /**
     * @brief Construct a new CLI::CLI object and parse all user arguments.
     *
     * @param argc Argument count.
     * @param argv Array of argument strings.
     *
     * @author Brock Hoos <brock.hoos@colorado.edu>
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    CLI::CLI(const int argc, char *const argv[])
    {
        std::pair<std::pair<double, double>, std::pair<double, double>> geoStartRegion; /* Used in area simulations. */
        std::pair<std::pair<double, double>, std::pair<double, double>> geoGoalRegion;  /* Used in area simulations. */

        int option; /* Option for parsing each user argument. */
        while ((option = getopt_long(argc, argv, shortOptions, longOptions, nullptr)) != -1)
        {
            switch (option)
            {
            case 's': /* Start position. */
                if (isStartCoordinateSet())
                {
                    throw std::invalid_argument("Starting coordinate cannot be set more than once.");
                }
                geoStrStartCoordinate = optarg;
                geoStartPosition = parseGeoCoordinates(geoStrStartCoordinate);
                break;
            case 'e': /* End position. */
                if (isGoalCoordinateSet())
                {
                    throw std::invalid_argument("Goal coordinate cannot be set more than once.");
                }
                geoStrGoalCoordinate = optarg;
                geoGoalPosition = parseGeoCoordinates(geoStrGoalCoordinate);
                break;
            case 'a': /* Starting area. */
                if (isStartCoordinateSet())
                {
                    throw std::invalid_argument("Starting coordinate cannot be set more than once.");
                }
                geoStrStartArea = optarg;
                geoStartRegion = parseGeoArea(geoStrStartArea);
                geoStartPosition = getRandomPointInGeoArea(geoStartRegion.first, geoStartRegion.second);
                break;
            case 'b': /* Ending area. */
                if (isGoalCoordinateSet())
                {
                    throw std::invalid_argument("Goal coordinate cannot be set more than once.");
                }
                geoStrGoalArea = optarg;
                geoGoalRegion = parseGeoArea(geoStrGoalArea);
                geoGoalPosition = getRandomPointInGeoArea(geoGoalRegion.first, geoGoalRegion.second);
                break;
            case 'x': /* Starting pixel. */
                if (isStartCoordinateSet())
                {
                    throw std::invalid_argument("Starting coordinate cannot be set more than once.");
                }
                imgStrStartCoordinate = optarg;
                imgStartPosition = parsePixelCoordinates(imgStrStartCoordinate);
                break;
            case 'y': /* Ending pixel. */
                if (isGoalCoordinateSet())
                {
                    throw std::invalid_argument("Goal coordinate cannot be set more than once.");
                }
                imgStrGoalCoordinate = optarg;
                imgGoalPosition = parsePixelCoordinates(imgStrGoalCoordinate);
                break;
            case 'i': /* Input filepath. */
                geotiffFilepath = optarg;
                break;
            case 'o': /* Output filepath. */
                outputFilename = optarg;
                break;
            case 'n': /* Number of numIterations. */
                numIterations = std::stoi(optarg);
                if (numIterations <= 0)
                {
                    throw std::out_of_range("Iterations must be greater than 0.");
                }
                break;
            case 'p': /* Maximum tolerable slope. */
                maxSlopeTolerance = std::stof(optarg);
                if (maxSlopeTolerance < 0.0f || maxSlopeTolerance > 90.0f)
                {
                    throw std::out_of_range("Maximum slope tolerance must be within (0\u00B0 - 90\u00B0)");
                }
                break;
            case 'r': /* Preferred radius or buffer size. */
                pixelBuffer = std::stoi(optarg);
                if (pixelBuffer <= 0)
                {
                    throw std::out_of_range("Radius must be greater than 0.");
                }
                break;
            case 'j': /* Toggle JSON output. */
                makeJSON = true;
                break;
            case 'h': /* View help menu. */
                print_helper();
                throw std::runtime_error("User argument help menu requested.");
            default: /* Default for undefined inputs. */
                print_helper();
                throw std::invalid_argument("Invalid option: " + std::string(argv[optind - 1]));
            }
        }

        imgCRS = isCoordinateSet(imgStartPosition) && isCoordinateSet(imgGoalPosition);
        geoCRS = isCoordinateSet(geoStartPosition) && isCoordinateSet(geoGoalPosition);
        if (!(imgCRS || geoCRS))
        {
            throw std::invalid_argument("Invalid coordinate system mismatch. Start and goal coordinates must be (lat, long) geospatial or (x, y) image based.");
        }

        if (std::isnan(numIterations) || std::isnan(maxSlopeTolerance) || std::isnan(pixelBuffer))
        {
            throw std::invalid_argument("Iterations, Slope, and Radius must all be declared.");
        }
    }

    /**
     * @brief Takes in string input form user, parses coordinates for delimiter (',') and makes sure coordinates are in correct format. (For geographical coordinates of double type).
     *
     * @param inputCoordinate "<double>,<double>" coordinate string.
     * @return std::pair<double, double> (lat, long) coordinates.
     *
     * @author Brock Hoos <brock.hoos@colorado.edu>
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    std::pair<double, double> CLI::parseGeoCoordinates(const std::string inputCoordinate) const
    {
        const size_t commaPosition = inputCoordinate.find(',');
        if (commaPosition == std::string::npos)
        {
            throw std::invalid_argument("Invalid coordinate format: " + inputCoordinate);
        }
        const double latOrdinate = std::stod(inputCoordinate.substr(static_cast<size_t>(0), commaPosition));
        const double longOrdinate = std::stod(inputCoordinate.substr(commaPosition + static_cast<size_t>(1)));
        return std::pair<double, double>(latOrdinate, longOrdinate);
    }

    /**
     * @brief Takes in string input form user, parses coordinates for delimiter (',') and makes sure coordinates are in correct format. (For pixel coordinates of int type).
     *
     * @param inputCoordinate "<int>,<int>" coordinate string.
     * @return std::pair<int, int> (x, y) coordinates.
     *
     * @author Brock Hoos <brock.hoos@colorado.edu>
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    std::pair<int, int> CLI::parsePixelCoordinates(const std::string inputCoordinate) const
    {
        const size_t commaPosition = inputCoordinate.find(',');
        if (commaPosition == std::string::npos)
        {
            throw std::invalid_argument("Invalid coordinate format: " + inputCoordinate);
        }
        const int xOrdinate = std::stod(inputCoordinate.substr(static_cast<size_t>(0), commaPosition));
        const int yOrdinate = std::stod(inputCoordinate.substr(commaPosition + static_cast<size_t>(1)));
        return std::pair<int, int>(xOrdinate, yOrdinate);
    }

    /**
     * @brief Takes in two coordinates (corners of a regional area) and uses ',' as delimiter to correctly parse areas and check format.
     *
     * @details Areas must be passed to getRandomPointInGeoArea for simulator usage.
     *
     * @param input "<double>,<double>:<double>,<double>" regional ordinate string.
     * @return std::pair<std::pair<double, double>, std::pair<double, double>> Pair of (lat, long) coordinates.
     *
     * @author Brock Hoos <brock.hoos@colorado.edu>
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    std::pair<std::pair<double, double>, std::pair<double, double>> CLI::parseGeoArea(const std::string inputRegion) const
    {
        const size_t colon_pos = inputRegion.find(':');
        if (colon_pos == std::string::npos)
        {
            throw std::invalid_argument("Invalid area format: " + inputRegion);
        }
        const std::pair<double, double> coordinate1 = parseGeoCoordinates(inputRegion.substr(static_cast<size_t>(0), colon_pos));
        const std::pair<double, double> coordinate2 = parseGeoCoordinates(inputRegion.substr(colon_pos + static_cast<size_t>(1)));
        return std::pair<std::pair<double, double>, std::pair<double, double>>(coordinate1, coordinate2);
    }
}
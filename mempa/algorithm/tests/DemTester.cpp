#include "../src/dem-handler/DemHandler.hpp"
#include "../src/rover-simulator/RoverSimulator.hpp"
#include "../src/rover-pathfinding-module/dijkstras.hpp"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <cassert>
#include <utility>
#include <vector>

#define BASIC_DEMTEST false
#define BASIC_SIMTEST true

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <DEM Filepath> <Chunk Size>\n";
        return 1;
    }

    const char *demFilepath = argv[1];
    const int chunkSize = std::stoi(argv[2]);

#if BASIC_DEMTEST
    try
    {
        // Create the DemHandler object
        mempa::DemHandler marsRaster(demFilepath);

        // Example coordinates (latitude, longitude)
        std::pair<double, double> userCoordinates(-146.1011, 40.4233);
        std::pair<double, double> userCoordinates2(-151.0011, 37.5233);

        // Convert to image coordinates
        std::pair<int, int> imageCoordinates =
            marsRaster.transformCoordinates(userCoordinates);
        std::pair<int, int> imageCoordinates2 =
            marsRaster.transformCoordinates(userCoordinates2);

        std::pair<std::pair<int, int>, std::pair<int, int>> rectCoordinates(imageCoordinates, imageCoordinates2);

        // Read the elevation chunk
        std::vector<std::vector<float>> elevationDataChunk =
            marsRaster.readRectangleChunk(rectCoordinates, chunkSize);

        // Ensure pixel resolution is 200
        const double sizetest = marsRaster.getImageResolution();
        assert(sizetest == 200.0);

        const char *ci_env = std::getenv("CI");
        if (!ci_env) // If CI variable is not set
        {
            constexpr int lineWidth = 5;
            for (std::vector<float> elevationDataRow : elevationDataChunk)
            {
                for (float elevationDataValue : elevationDataRow)
                {
                    std::cout << std::setw(lineWidth) << elevationDataValue;
                }
                std::cout << "\n";
            }
        }
    }
    catch (const std::exception &demError)
    {
        std::cerr << "Error: " << demError.what() << "\n";
        return 1;
    }
#endif

#if BASIC_SIMTEST
    try
    {
        // Make the DEM object.
        mempa::DemHandler marsRaster(demFilepath);

        // Example coordinates (latitude, longitude)
        std::pair<double, double> userCoordinates1(-150., 40.);
        std::pair<double, double> userCoordinates2(-151., 39.);

        // Make the siulator object.
        mempa::RoverSimulator marsSimulator(&marsRaster, userCoordinates1, userCoordinates2);

        // Create a new pathfinding algorithm object.
        Dijkstras marsDisjkstrasAlgorithm;

        std::vector<std::pair<int, int>> routedPath = marsSimulator.runSimulator(&marsDisjkstrasAlgorithm, chunkSize);

        const char *ci_env = std::getenv("CI");
        if (!ci_env) // If CI variable is not set
        {
            for (std::pair<int, int> pathCoordinate : routedPath)
            {
                std::cout << "(" << pathCoordinate.first << ", " << pathCoordinate.second << ")\n";
            }
            std::cout << '\n';
            for (std::pair<int, int> pathCoordinate : routedPath)
            {
                std::pair<double, double> geoCoordinate = marsRaster.revertCoordinates(pathCoordinate);
                std::cout << std::fixed << std::setprecision(4) << "(" << geoCoordinate.first << ", " << geoCoordinate.second << ")\n";
            }
        }
    }
    catch (const std::exception &demError)
    {
        std::cerr << "Error: " << demError.what() << "\n";
        return 1;
    }
#endif

    return 0;
}

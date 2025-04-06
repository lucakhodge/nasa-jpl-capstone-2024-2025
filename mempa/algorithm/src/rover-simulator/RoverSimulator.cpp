/* Local Header */
#include "RoverSimulator.hpp"

/* mempa::DemHandler */
#include "../dem-handler/DemHandler.hpp"

/* SearchAlgorithms */
#include "../rover-pathfinding-module/SearchAlgorithm.hpp"
#include "../rover-pathfinding-module/SearchContext.hpp"

/* C++ Standard Libraries */
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace mempa
{
#if PREPROCESS_SLOPE
    /**
     * @brief Construct a new Rover Simulator:: Rover Simulator object
     *
     * @param elevationRaster Pointer to the DemHandler object for the elevation raster.
     * @param slopeRaster Pointer to the DemHandler object for the slope raster.
     * @param startPosition Pair of doubles for the geographic coordinate of the starting position.
     * @param endPosition Pair of doubles for the geographic coordinate of the destination.
     */
    RoverSimulator::RoverSimulator(const DemHandler *elevationRaster, const DemHandler *slopeRaster, const std::pair<double, double> startPosition, const std::pair<double, double> endPosition) noexcept
        : elevationRaster(elevationRaster), slopeRaster(slopeRaster), imageResolution(elevationRaster->getImageResolution()),
          startPosition(elevationRaster->transformCoordinates(startPosition)), endPosition(elevationRaster->transformCoordinates(endPosition))
    {
        /* The rover should currently be at the start position. */
        currentPosition = this->startPosition;
    }
#else
    /**
     * @brief Construct a new Rover Simulator:: Rover Simulator object
     *
     * @param elevationRaster Pointer to the DemHandler object for the elevation raster.
     * @param startPosition Pair of doubles for the geographic coordinate of the starting position.
     * @param endPosition Pair of doubles for the geographic coordinate of the destination.
     */
    RoverSimulator::RoverSimulator(const DemHandler *elevationRaster, const std::pair<double, double> startPosition, const std::pair<double, double> endPosition) noexcept
        : elevationRaster(elevationRaster), imageResolution(elevationRaster->getImageResolution()),
          startPosition(elevationRaster->transformCoordinates(startPosition)), endPosition(elevationRaster->transformCoordinates(endPosition))
    {
        /* The rover should currently be at the start position. */
        currentPosition = this->startPosition;
    }
#endif

    /**
     * @brief Run the simulator for square chunk views
     *
     * @param algorithmType @ref SearchAlgorithm class to use for pathfinding.
     * @param buffer How much to buffer the space around the coordinate by.
     * @return std::vector<std::pair<int, int>>
     */
    std::vector<std::pair<int, int>>
    RoverSimulator::runSimulator(SearchAlgorithm *algorithmType, const int buffer)
    {
        SearchContext roverRouter(algorithmType);                                                                                                   /* Set up the pathfinding algorithm based on input. */
        std::vector<std::pair<int, int>> routedRasterPath = {currentPosition};                                                                      /* Holds the coordinates of every traversed area of the raster. */
        routedRasterPath.reserve(std::max(std::abs(endPosition.first - startPosition.first), std::abs(endPosition.second - startPosition.second))); /* Reserve enough memory in the vector for a straight line. */

        do
        {
            std::pair<int, int> vectorPosition;                                                                                        /* Will be updated to relative (currentPosition, endPosition) coordinates within the vector. */
            std::vector<std::vector<float>> elevationMap = elevationRaster->readSquareChunk(currentPosition, buffer, &vectorPosition); /* The elevation data we read from the DemHandler elevationRaster. */
            std::pair<int, int> chunkLocation = globalVectorCorner(currentPosition, buffer);                                           /* Contains the (0, 0) position in the vector as a globally spaced coordinate. */

            /* Initialize a new setup for the pathfinding algorithm. */
            roverRouter.executeStrategyReset();                                                                                                                       /* Reset the algorithm. */
            roverRouter.executeStrategySetUpAlgo(elevationMap, chunkLocation, currentPosition, endPosition, MAX_SLOPE, imageResolution);                              /* Set up the algorithm for the new view. */
            roverRouter.executeStrategyGetStep(elevationMap, chunkLocation, currentPosition, endPosition, MAX_SLOPE, imageResolution);                                /* It still does the thing where it returns the currentPosition first. */
            std::pair<int, int> pathStep = roverRouter.executeStrategyGetStep(elevationMap, chunkLocation, currentPosition, endPosition, MAX_SLOPE, imageResolution); /* Get the first step made by the algorithm. */

            /* Add the step made to the route and update current position. */
            routedRasterPath.push_back(pathStep);
            currentPosition = pathStep;
        } while (currentPosition != endPosition);

        return routedRasterPath;
    }
} // namespace mempa

/* Local Header */
#include "RoverSimulator.hpp"

/* mempa::DemHandler */
#include "../DemHandler/DemHandler.hpp"

/* SearchAlgorithms */
#include "../search_algorithms/SearchContext.h"
#include "../search_algorithms/SearchAlgorithm.h"

/* C++ Standard Libraries */
#include <cmath>
#include <algorithm>
#include <vector>
#include <utility>

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
        : elevationRaster(elevationRaster), slopeRaster(slopeRaster), imageResolution(elevationRaster->getImageResolution()), startPosition(elevationRaster->transformCoordinates(startPosition)), endPosition(elevationRaster->transformCoordinates(endPosition))
    {
        /* The rover should currently be at the start position. */
        currentPosition = this->startPosition;
    }
#else
    /**
     * @brief Construct a new Rover Simulator:: Rover Simulator object
     *
     * @param elevationRaster Pointer to the DemHandler object for the elevation raster.
     * @param slopeRaster Pointer to the DemHandler object for the slope raster.
     * @param startPosition Pair of doubles for the geographic coordinate of the starting position.
     * @param endPosition Pair of doubles for the geographic coordinate of the destination.
     */
    RoverSimulator::RoverSimulator(const DemHandler *elevationRaster, const std::pair<double, double> startPosition, const std::pair<double, double> endPosition) noexcept
        : elevationRaster(elevationRaster), imageResolution(elevationRaster->getImageResolution()), startPosition(elevationRaster->transformCoordinates(startPosition)), endPosition(elevationRaster->transformCoordinates(endPosition))
    {
        /* The rover should currently be at the start position. */
        currentPosition = this->startPosition;
    }
#endif

    /**
     * @brief Run the simulator for sqiare chunk views
     *
     * @param algorithmType @ref SearchAlgorithm class to use for pathfinding.
     * @param buffer How much to buffer the space around the coordinate by.
     * @return std::vector<std::pair<int, int>>
     */
    std::vector<std::pair<int, int>> RoverSimulator::runSimulator(SearchAlgorithm *algorithmType, const int buffer)
    {
        SearchContext roverRouter(algorithmType);                              /* Set up the pathfinding algorithm based on input. */
        std::vector<std::pair<int, int>> routedRasterPath = {currentPosition}; /* Holds the coordinates of every traversed area of the raster. */
        routedRasterPath.reserve(std::max(std::abs(endPosition.first - startPosition.first), std::abs(endPosition.second - startPosition.second)));

        do
        {
            std::pair<int, int> vectorPosition;                                                                                        /* Will be updated to relative (currentPosition, endPosition) coordinates within the vector. */
            std::vector<std::vector<float>> elevationMap = elevationRaster->readSquareChunk(currentPosition, buffer, &vectorPosition); /* The elevation data we read from the DemHandler elevationRaster. */
            std::pair<int, int> currentDistance = coordinateDifference(currentPosition, endPosition);
            std::pair<int, int> vectorGoal(std::max(-buffer, std::min(buffer, endPosition.first)), std::max(-buffer, std::min(buffer, endPosition.second)));

            roverRouter.executeStrategyReset();
            roverRouter.executeStrategySetUpAlgo(elevationMap, vectorPosition, vectorGoal, MAX_SLOPE, imageResolution);

            std::pair<int, int> algorithmStep = roverRouter.executeStrategyGetStep();
            if (algorithmStep == BREAK_STEP)
            {
                currentPosition = endPosition;
                routedRasterPath.push_back(currentPosition);
            }
            else
            {
                std::pair<int, int> stepDifference = coordinateDifference(vectorPosition, algorithmStep);
                currentPosition = std::pair<int, int>(currentPosition.first + stepDifference.first, currentPosition.second + stepDifference.second);
                routedRasterPath.push_back(currentPosition);
            }
        } while (currentPosition != endPosition);

        return routedRasterPath;
    }
}

#include "RoverSimulator.h"
#include "../DemHandler/DemHandler.h"
#include <utility>

namespace mempa
{

    /**
     * @brief Construct a new Rover Simulator:: Rover Simulator object
     *
     * @param elevationRaster
     * @param slopeRaster
     * @param startPosition
     * @param endPosition
     */
    RoverSimulator::RoverSimulator(const DemHandler *elevationRaster, const DemHandler *slopeRaster, const std::pair<double, double> startPosition, const std::pair<double, double> endPosition) noexcept
        : elevationRaster(elevationRaster), slopeRaster(slopeRaster), startPosition(elevationRaster->transformCoordinates(startPosition)), endPosition(elevationRaster->transformCoordinates(endPosition))
    {
        /* The rover should currently be at the start position. */
        currentPosition = this->startPosition;
    }

    void RoverSimulator::runSimulator(SearchAlgorithm *pathfinder)
    {
        constexpr int READ_BUFFER = 10; /* How much of the raster to be read around the coordinates. */
        while (currentPosition != endPosition)
        {
            std::pair<std::pair<int, int>, std::pair<int, int>> regionCoordinates = std::make_pair(currentPosition, endPosition);                   /* The current position coordinate and the goal. */
            std::pair<std::pair<int, int>, std::pair<int, int>> vectorCoordinates;                                                                  /* Will be updated to relative (currentPosition, endPosition) coordinates within the vector. */
            std::vector<std::vector<float>> elevationMap = elevationRaster->readRectangleChunk(regionCoordinates, READ_BUFFER, &vectorCoordinates); /* The elevation data we read from the DemHandler elevationRaster. */
            std::vector<std::vector<float>> slopeData = slopeRaster->readRectangleChunk(regionCoordinates, READ_BUFFER, nullptr);                   /* Corresponding slope values for each point of elevation data. */
        }
    }
}

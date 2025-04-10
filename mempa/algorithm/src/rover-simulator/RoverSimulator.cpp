/* Local Header */
#include "RoverSimulator.hpp"

/* mempa::DemHandler */
#include "../dem-handler/DemHandler.hpp"

/* SearchAlgorithms */
#include "../rover-pathfinding-module/SearchAlgorithm.hpp"

/* C++ Standard Libraries */
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

namespace mempa {
#if PREPROCESS_SLOPE
/**
 * @brief Construct a new Rover Simulator:: Rover Simulator object
 *
 * @param elevationRaster Pointer to the DemHandler object for the elevation
 * raster.
 * @param slopeRaster Pointer to the DemHandler object for the slope raster.
 * @param startPosition Pair of doubles for the geographic coordinate of the
 * starting position.
 * @param goalPosition Pair of doubles for the geographic coordinate of the
 * destination.
 *
 * @author Ryan Wagster <rywa2447@colorado.edu>
 */
RoverSimulator::RoverSimulator(
    const DemHandler *elevationRaster, const DemHandler *slopeRaster,
    const std::pair<double, double> startPosition,
    const std::pair<double, double> goalPosition) noexcept
    : elevationRaster(elevationRaster), slopeRaster(slopeRaster),
      imageResolution(elevationRaster->getImageResolution()),
      startPosition(elevationRaster->transformCoordinates(startPosition)),
      goalPosition(elevationRaster->transformCoordinates(goalPosition)) {
  /* The rover should currently be at the start position. */
  currentPosition = this->startPosition;
}
#else
/**
 * @brief Construct a new Rover Simulator:: Rover Simulator object
 *
 * @param elevationRaster Pointer to the DemHandler object for the elevation
 * raster.
 * @param startPosition Pair of doubles for the geographic coordinate of the
 * starting position.
 * @param goalPosition Pair of doubles for the geographic coordinate of the
 * destination.
 *
 * @author Ryan Wagster <rywa2447@colorado.edu>
 */
RoverSimulator::RoverSimulator(
    const DemHandler *elevationRaster,
    const std::pair<double, double> startPosition,
    const std::pair<double, double> goalPosition) noexcept
    : elevationRaster(elevationRaster),
      imageResolution(this->elevationRaster->getImageResolution()),
      startPosition(startPosition), goalPosition(goalPosition) {
  /* The rover should currently be at the start position. */
  currentPosition = this->startPosition;
}
#endif

/**
 * @brief Run the simulator for square chunk views
 *
 * @param algorithmType @ref SearchAlgorithm class to use for pathfinding.
 * @param max_slope Maximum tolerable slope for the rover.
 * @param buffer How much to buffer the space around the coordinate by.
 * @return std::vector<std::pair<int, int>> Route taken from start coordinate to
 * destination.
 *
 * @author Ryan Wagster <rywa2447@colorado.edu>
 */
std::vector<std::pair<int, int>>
RoverSimulator::runSimulator(SearchAlgorithm *algorithm, const float max_slope,
                             const int buffer) {
  std::vector<std::pair<int, int>> routedRasterPath = {
      currentPosition}; /* Holds the coordinates of every traversed area of the
                           raster. */
  routedRasterPath.reserve(std::max(
      std::abs(goalPosition.first - startPosition.first),
      std::abs(goalPosition.second -
               startPosition.second))); /* Reserve enough memory in the vector
                                           for a straight line. */

  do {
    std::pair<int, int>
        vectorPosition; /* Will be updated to relative (currentPosition,
                           goalPosition) coordinates within the vector. */
    std::vector<std::vector<float>> elevationMap =
        elevationRaster->readSquareChunk(
            currentPosition, buffer,
            &vectorPosition); /* The elevation data we read from the DemHandler
                                 elevationRaster. */
    std::pair<int, int> chunkLocation = globalVectorCorner(
        currentPosition, buffer); /* Contains the (0, 0) position in the vector
                                     as a globally spaced coordinate. */

    std::cout << "BEFORE GET STEP" << std::endl;
    std::vector<std::pair<int, int>> pathSegment =
        algorithm->get_step(elevationMap, chunkLocation, currentPosition,
                            goalPosition, max_slope, imageResolution);
    std::cout << "AFTER GET STEP " << pathSegment.size() << std::endl;

    for (auto &pathStep : pathSegment) {
      /* Add the step made to the route and update current position. */
      routedRasterPath.push_back(pathStep);
      currentPosition = pathStep;
      std::cout << "CURRENT POS: (" << currentPosition.first << ","
                << currentPosition.second << ")" << std::endl;
    }

  } while (currentPosition != goalPosition);

  return routedRasterPath;
}
} // namespace mempa

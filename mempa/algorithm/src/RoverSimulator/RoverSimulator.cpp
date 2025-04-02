/* Local Header */
#include "../RoverSimulator/RoverSimulator.hpp"

/* mempa::DemHandler */
#include "../DemHandler/DemHandler.hpp"

/* C++ Standard Libraries */
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace mempa {

/**
 * @brief Construct a new Rover Simulator:: Rover Simulator object
 *
 * @param elevationRaster Pointer to the DemHandler object for the elevation
 * raster.
 * @param slopeRaster Pointer to the DemHandler object for the slope raster.
 * @param startPosition Pair of doubles for the geographic coordinate of the
 * starting position.
 * @param endPosition Pair of doubles for the geographic coordinate of the
 * destination.
 */
RoverSimulator::RoverSimulator(
    const DemHandler *elevationRaster, const DemHandler *slopeRaster,
    const std::pair<double, double> startPosition,
    const std::pair<double, double> endPosition) noexcept
    : elevationRaster(elevationRaster), slopeRaster(slopeRaster),
      startPosition(elevationRaster->transformCoordinates(startPosition)),
      endPosition(elevationRaster->transformCoordinates(endPosition)) {
  /* The rover should currently be at the start position. */
  currentPosition = this->startPosition;
}

std::vector<std::pair<int, int>>
RoverSimulator::runSimulator(const int buffer) {
  std::vector<std::pair<int, int>>
      routedPath; /* Holds the coordinates of every traversed area of the
                     raster. */
  routedPath.reserve(
      std::max(std::abs(endPosition.first - startPosition.first),
               std::abs(endPosition.second - startPosition.second)));

  do {
#if ROVERSIMULATOR_RECTANGLE
    std::pair<std::pair<int, int>, std::pair<int, int>> regionCoordinates =
        std::make_pair(
            currentPosition,
            endPosition); /* The current position coordinate and the goal. */
    std::pair<std::pair<int, int>, std::pair<int, int>>
        vectorCoordinates; /* Will be updated to relative (currentPosition,
                              endPosition) coordinates within the vector. */
    std::vector<std::vector<float>> elevationMap =
        elevationRaster->readRectangleChunk(
            regionCoordinates, buffer,
            &vectorCoordinates); /* The elevation data we read from the
                                    DemHandler elevationRaster. */
    std::vector<std::vector<float>> slopeMap = slopeRaster->readRectangleChunk(
        regionCoordinates, buffer); /* Corresponding slope values for each point
                                       of elevation data. */

    if (!(validateCoordinate(vectorCoordinates.first, elevationMap) &&
          validateCoordinate(vectorCoordinates.second, elevationMap))) {
      throw std::runtime_error(
          "runSimulator: startPosition or endPosition out of bounds");
    }

    while (true) {
      // std::pair<int, int> pathfinderVectorStep =
    }

#endif
  } while (currentPosition != endPosition);

  return routedPath;
}
} // namespace mempa

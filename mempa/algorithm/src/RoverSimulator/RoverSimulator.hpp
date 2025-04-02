#pragma once
#define ROVERSIMULATOR_RECTANGLE true
#define ROVERSIMULATOR_SQUARE false
#define ROVERSIMULATOR_CIRCLE false

/* mempa::DemHandler */
#include "../DemHandler/DemHandler.hpp"

/* mempa::SearchAlgorithm */
#include "../search_algorithms/SearchAlgorithm.h"

/* C++ Standard Libraries */
#include <cmath>
#include <utility>
#include <vector>

namespace mempa {
/**
 * @brief Attempt at a revision of the RoverSimulator file.
 *
 * @author Ryan Wagster <rywa2447@colorado.edu>
 */
class RoverSimulator {
private:
  const DemHandler *
      elevationRaster; /* Handler for the DEM file containing elevation data. */
  const DemHandler *
      slopeRaster; /* Handler for the Slope raster derived from the DEM data. */
  const std::pair<int, int>
      startPosition; /* The rover's initial image-based coordinate position. */
  const std::pair<int, int>
      endPosition; /* The rover's image-based coordinate desination. */
  std::pair<int, int> currentPosition; /* The rover's current image-based
                                          coordinate position. */
  static inline constexpr std::pair<int, int> breakStep =
      std::make_pair(-1, -1); /* The value that a pathfinding algorithm returns
                                 when it is complete. */
  inline bool validateCoordinate(
      std::pair<int, int> vecCoordinate,
      std::vector<std::vector<float>> rasterVector) const noexcept;

protected:
  /* RoverSimulator is not designed to be subclassed. */

public:
  explicit RoverSimulator(const DemHandler *elevationRaster,
                          const DemHandler *slopeRaster,
                          std::pair<double, double> startPosition,
                          std::pair<double, double> endPosition) noexcept;
  std::vector<std::pair<int, int>> runSimulator(int buffer);
  inline bool validateElevation(float elevationValue) const noexcept;
};
} // namespace mempa

#include "RoverSimulator.inl"

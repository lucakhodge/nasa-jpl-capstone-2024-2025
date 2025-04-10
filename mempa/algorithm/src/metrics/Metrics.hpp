#pragma once

#include "../dem-handler/DemHandler.hpp"
#include <utility>
#include <vector>

/**
 * @brief Stores comprehensive terrain metrics for path analysis
 *
 * @author Adam Carlson
 */
class Metrics {
private:
  void reset();

public:
  float totalDistance = 0.0;
  float horizontalDistance = 0.0;
  float asTheCrowFlysDistance = 0.0;
  float totalElevationChange = 0.0;
  float netElevationChange = 0.0;
  float maxSlope = 0.0;
  float averageSlope = 0.0;
  float energyCost = 0.0;

  Metrics() {}

  void analizePath(std::vector<std::pair<int, int>> path);
};

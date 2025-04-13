#include "Metrics.hpp"
#include "../dem-handler/DemHandler.hpp"
#include <math.h>

void Metrics::analizePath(std::vector<std::pair<int, int>> path) {
  this->reset();
  for (int i = 1; i < path.size(); i++) {
    std::pair<int, int> before = path[i - 1];
    std::pair<int, int> after = path[i];
    this->horizontalDistance +=
        std::sqrt(std::pow(after.first - before.first, 2) +
                  std::pow(after.second - before.second, 2));
  }

  std::pair<int, int> startCoordinate = path.front();
  std::pair<int, int> endCoordinate = path.back();
  this->asTheCrowFlysDistance =
      std::sqrt(std::pow(startCoordinate.first - endCoordinate.first, 2) +
                std::pow(startCoordinate.second - endCoordinate.second, 2));
}

void Metrics::reset() {
  totalDistance = 0.0;
  horizontalDistance = 0.0;
  asTheCrowFlysDistance = 0.0;
  totalElevationChange = 0.0;
  netElevationChange = 0.0;
  maxSlope = 0.0; // calcualted off of totalElevationChange
  averageSlope = 0.0; // calculated off of totalElevationChange
}
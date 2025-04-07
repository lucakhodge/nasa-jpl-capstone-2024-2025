#include "TerrainMetrics.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <typeinfo>
#include <unistd.h>
#include <vector>

/**
 * @brief Class to hold Terrain Metrics to display information discovered about
 * the path to the user
 *
 * @author Adam Carlson
 */
class TerrainMetrics {

  // Constants for Mars terrain analysis
  const float MARS_PIXEL_SIZE = 200.0; // Mars DEM resolution (meters)
  const float MAX_SLOPE = 25.0;        // Maximum allowed slope (degrees)
  const float MM_TO_M = 0.001;         // Millimeter to meter conversion
  const uint32_t TERRAIN_PADDING = 15; // Padding for path finding
  const float MAX_ELEVATION_CHANGE =
      50.0; // Maximum elevation change per pixel (meters)
  const float ELEVATION_EPSILON =
      0.1; // Small elevation difference threshold (meters)
  const float MIN_MARS_ELEVATION = -8500.0; // Minimum Mars elevation (meters)
  const float MAX_MARS_ELEVATION = 21230.0; // Maximum Mars elevation (meters)
  const float ELEVATION_SCALING = 0.001;    // Scale factor for large elevations
  const float SIGNIFICANT_CHANGE =
      10.0; // Significant elevation change threshold (meters)
public:
  float totalDistance = 0.0;
  float horizontalDistance = 0.0;
  float totalElevationChange = 0.0;
  float netElevationChange = 0.0;
  float maxSlope = 0.0;
  float averageSlope = 0.0;
  float energyCost = 0.0;
  size_t waypointCount = 0;
  std::vector<float> slopes;
  float baseElevation = 0.0;
  bool baseElevationSet = false;
  std::vector<std::vector<float>> relativeHeights;

  /**
   * @brief Function to set up TerrainMetrics class for use.
   *
   * @author Adam Carlson
   */
  void initialize(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
      throw std::runtime_error("Invalid dimensions for terrain metrics");
    }
    reset();
    relativeHeights.resize(rows, std::vector<float>(cols, 0.0));
  }

  /**
   * @brief Resets all of the TerrianMetric class values to their default
   * values.
   *
   * @author Adam Carlson
   */
  void reset() {
    totalDistance = 0.0;
    horizontalDistance = 0.0;
    totalElevationChange = 0.0;
    netElevationChange = 0.0;
    maxSlope = 0.0;
    averageSlope = 0.0;
    energyCost = 0.0;
    waypointCount = 0;
    slopes.clear();
    baseElevation = 0.0;
    baseElevationSet = false;
    relativeHeights.clear();
  }

  /**
   * @brief A check to see if the TerrianMetrics class has been initialized
   *
   * @author Adam Carlson
   */
  bool isInitialized() const {
    return !relativeHeights.empty() && !relativeHeights[0].empty();
  }

  /**
   * @brief calculates a value to represent the energy cost of a path.  Takes
   * into account distance and elevation change
   *
   * @author Adam Carlson
   */
  float calculateEnergyCost(float distance, float elevChange, float slope) {
    float slopePenalty = std::pow(slope / MAX_SLOPE, 2);
    float elevationPenalty = std::abs(elevChange) * 2.0;
    return distance + elevationPenalty + (distance * slopePenalty);
  }
};

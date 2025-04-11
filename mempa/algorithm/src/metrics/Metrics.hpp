#pragma once

#include "../dem-handler/DemHandler.hpp"
#include <utility>
#include <vector>
#include <tuple>

namespace mempa {
  class DemHandler;
}

/**
 * @brief Stores comprehensive terrain metrics for path analysis
 *
 * @author Adam Carlson
 */
class Metrics {
public:
  /**
   * @brief Default constructor
   */
  Metrics() { reset(); }
  
  /**
   * @brief Reset all metrics to zero
   */
  void reset();
  
  /**
   * @brief Calculate individual metrics
   */
  void calculateHorizontalDistance(const std::vector<std::pair<int, int>>& path);
  void calculateCrowFliesDistance(const std::vector<std::pair<int, int>>& path);
  void calculateTotalDistance(const std::vector<std::pair<int, int>>& path, 
                            const mempa::DemHandler* demHandler);
  void calculateTotalElevationChange(const std::vector<std::pair<int, int>>& path,
                                   const mempa::DemHandler* demHandler);
  void calculateNetElevationChange(const std::vector<std::pair<int, int>>& path,
                                 const mempa::DemHandler* demHandler);
  void calculateMaxSlope(const std::vector<std::pair<int, int>>& path,
                       const mempa::DemHandler* demHandler);
  void calculateAverageSlope(const std::vector<std::pair<int, int>>& path,
                           const mempa::DemHandler* demHandler);
  
  /**
   * @brief Legacy path analysis method (note: maintains misspelling for backward compatibility)
   * @param path Vector of coordinate pairs representing the path
   */
  void analizePath(std::vector<std::pair<int, int>> path);
  
  /**
   * @brief Comprehensive path analysis with elevation data
   * @param path Vector of coordinate pairs representing the path
   * @param demHandler Pointer to DEM handler for elevation data
   */
  void analyzePath(const std::vector<std::pair<int, int>>& path, 
                 const mempa::DemHandler* demHandler);
  
  // Distance metrics
  float totalDistance = 0.0;         // 3D distance including elevation
  float horizontalDistance = 0.0;    // 2D distance ignoring elevation
  float asTheCrowFlysDistance = 0.0; // Direct line from start to end
  
  // Elevation metrics
  float totalElevationChange = 0.0;  // Sum of absolute elevation changes
  float netElevationChange = 0.0;    // End elevation minus start elevation
  
  // Slope metrics
  float maxSlope = 0.0;              // Maximum slope along path (degrees)
  float averageSlope = 0.0;          // Average slope along path (degrees)
  
  // Energy metrics
  float energyCost = 0.0;            // Estimated energy required for path
};
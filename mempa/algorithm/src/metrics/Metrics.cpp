#include "Metrics.hpp"
#include "../dem-handler/DemHandler.hpp"
#include <math.h>
#include <algorithm>
#include <iostream>
#include <iomanip> // For debug formatting

void Metrics::reset() {
  totalDistance = 0.0;
  horizontalDistance = 0.0;
  asTheCrowFlysDistance = 0.0;
  totalElevationChange = 0.0;
  netElevationChange = 0.0;
  maxSlope = 0.0;
  averageSlope = 0.0;
}

void Metrics::analizePath(std::vector<std::pair<int, int>> path) {
  this->reset();
  for (size_t i = 1; i < path.size(); i++) {
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
                
  // Set totalDistance to horizontalDistance by default
  this->totalDistance = this->horizontalDistance;
}

void Metrics::calculateTotalDistance(const std::vector<std::pair<int, int>>& path, 
                                   const mempa::DemHandler* demHandler) {
  this->totalDistance = 0.0;
  
  if (!demHandler) {
    std::cerr << "DEM handler is null in calculateTotalDistance" << std::endl;
    this->totalDistance = this->horizontalDistance;
    return;
  }
  
  // Check if path is valid
  if (path.size() < 2) {
    this->totalDistance = 0.0;
    return;
  }
  
  std::cout << "=============== ELEVATION DEBUG INFO ===============" << std::endl;
  std::cout << "Path start point: (" << path[0].first << ", " << path[0].second << ")" << std::endl;
  std::cout << "Path end point: (" << path.back().first << ", " << path.back().second << ")" << std::endl;
  
  // Print a few sample elevations to verify the DEM is providing meaningful values
  bool hasElevationData = false;
  float firstElev = 0.0f;
  
  try {
    // Sample a few points along the path
    for (int i = 0; i < std::min(5, static_cast<int>(path.size())); i++) {
      int idx = i * path.size() / 5;  // spaced out indices
      float elev = demHandler->getValue(path[idx].first, path[idx].second);
      std::cout << "Sample elevation at point " << idx << " (" 
                << path[idx].first << ", " << path[idx].second << "): " 
                << std::fixed << std::setprecision(2) << elev << std::endl;
      
      if (i == 0) {
        firstElev = elev;
      } else if (std::abs(elev - firstElev) > 0.01) {
        // If we find at least one elevation that differs from the first, we have variable terrain
        hasElevationData = true;
      }
    }
    
    if (!hasElevationData) {
      std::cout << "WARNING: All sampled elevations are identical or very similar!" << std::endl;
      std::cout << "This may indicate a flat DEM or issue with elevation data." << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error accessing sample DEM data: " << e.what() << std::endl;
    this->totalDistance = this->horizontalDistance;
    return;
  }
  
  // Now calculate the total 3D distance
  for (size_t i = 1; i < path.size(); i++) {
    std::pair<int, int> before = path[i - 1];
    std::pair<int, int> after = path[i];
    
    // Calculate horizontal component
    float horizontalDist = std::sqrt(
        std::pow(after.first - before.first, 2) +
        std::pow(after.second - before.second, 2));
    
    // Get elevation data
    float elevBefore = 0.0f;
    float elevAfter = 0.0f;
    
    try {
      elevBefore = demHandler->getValue(before.first, before.second);
      elevAfter = demHandler->getValue(after.first, after.second);
    } catch (const std::exception& e) {
      std::cerr << "Error getting elevation at segment " << i << ": " << e.what() << std::endl;
      // Fall back to horizontal distance for this segment
      this->totalDistance += horizontalDist;
      continue;
    }
    
    // Debug output for a few points to verify calculations
    if (i < 3 || i > path.size() - 3) {  // First and last few points
      std::cout << "Segment " << i << ": (" << before.first << "," << before.second 
                << ") to (" << after.first << "," << after.second << ")" << std::endl;
      std::cout << "  Elevations: " << elevBefore << " to " << elevAfter 
                << " (diff: " << elevAfter - elevBefore << ")" << std::endl;
    }
    
    float elevDiff = elevAfter - elevBefore;
    
    // Calculate 3D distance using Pythagorean theorem
    float segmentDist = std::sqrt(horizontalDist*horizontalDist + elevDiff*elevDiff);
    
    // If there's a significant difference between 2D and 3D distance, note it
    if (std::abs(segmentDist - horizontalDist) > 0.1) {
      std::cout << "  Significant elevation effect: 2D=" << horizontalDist 
                << ", 3D=" << segmentDist << std::endl;
    }
    
    this->totalDistance += segmentDist;
  }
  
  // Compare 2D and 3D distances to see if elevation had any effect
  std::cout << "Total 2D distance: " << this->horizontalDistance << std::endl;
  std::cout << "Total 3D distance: " << this->totalDistance << std::endl;
  std::cout << "Difference: " << this->totalDistance - this->horizontalDistance << std::endl;
  std::cout << "=====================================================" << std::endl;
}

void Metrics::calculateTotalElevationChange(const std::vector<std::pair<int, int>>& path,
                                          const mempa::DemHandler* demHandler) {
  this->totalElevationChange = 0.0;
  
  if (!demHandler || path.size() < 2) {
    return;
  }
  
  for (size_t i = 1; i < path.size(); i++) {
    std::pair<int, int> before = path[i - 1];
    std::pair<int, int> after = path[i];
    
    float elevBefore = 0.0f;
    float elevAfter = 0.0f;
    
    try {
      elevBefore = demHandler->getValue(before.first, before.second);
      elevAfter = demHandler->getValue(after.first, after.second);
      
      // Add absolute elevation change for this segment
      float change = std::abs(elevAfter - elevBefore);
      this->totalElevationChange += change;
      
      // Debug output for significant changes
      if (change > 1.0) {
        std::cout << "Significant elevation change at segment " << i 
                  << ": " << change << " units" << std::endl;
      }
    } catch (const std::exception& e) {
      continue; // Skip segments with invalid elevation data
    }
  }
  
  std::cout << "Total elevation change (sum of all ups and downs): " 
            << this->totalElevationChange << std::endl;
}

void Metrics::calculateNetElevationChange(const std::vector<std::pair<int, int>>& path,
                                        const mempa::DemHandler* demHandler) {
  this->netElevationChange = 0.0;
  
  if (!demHandler || path.size() < 2) {
    return;
  }
  
  std::pair<int, int> startPoint = path.front();
  std::pair<int, int> endPoint = path.back();
  
  float startElev = 0.0f;
  float endElev = 0.0f;
  
  try {
    startElev = demHandler->getValue(startPoint.first, startPoint.second);
    endElev = demHandler->getValue(endPoint.first, endPoint.second);
    this->netElevationChange = endElev - startElev;
    
    std::cout << "Net elevation change (end-start): " 
              << endElev << " - " << startElev << " = " 
              << this->netElevationChange << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error calculating net elevation change: " << e.what() << std::endl;
    return;
  }
}

void Metrics::calculateMaxSlope(const std::vector<std::pair<int, int>>& path,
                              const mempa::DemHandler* demHandler) {
  this->maxSlope = 0.0;
  
  if (!demHandler || path.size() < 2) {
    return;
  }
  
  for (size_t i = 1; i < path.size(); i++) {
    std::pair<int, int> before = path[i - 1];
    std::pair<int, int> after = path[i];
    
    // Calculate horizontal distance
    float horizontalDist = std::sqrt(
        std::pow(after.first - before.first, 2) +
        std::pow(after.second - before.second, 2));
    
    // Skip calculations for very small horizontal distances to avoid division by near-zero
    if (horizontalDist < 0.001) {
      continue;
    }
    
    // Get elevation data
    float elevBefore = 0.0f;
    float elevAfter = 0.0f;
    
    try {
      elevBefore = demHandler->getValue(before.first, before.second);
      elevAfter = demHandler->getValue(after.first, after.second);
      
      float elevDiff = std::abs(elevAfter - elevBefore);
      
      // Calculate slope in degrees (division by zero is already checked)
      float segmentSlope = std::atan(elevDiff / horizontalDist) * (180.0 / M_PI);
      
      // Track the steepest segment
      if (segmentSlope > this->maxSlope) {
        this->maxSlope = segmentSlope;
        std::cout << "New max slope at segment " << i << ": " 
                  << segmentSlope << " degrees (elev diff: " << elevDiff 
                  << ", horiz dist: " << horizontalDist << ")" << std::endl;
      }
    } catch (const std::exception& e) {
      continue;
    }
  }
  
  std::cout << "Maximum slope along path: " << this->maxSlope << " degrees" << std::endl;
}

void Metrics::calculateAverageSlope(const std::vector<std::pair<int, int>>& path,
                                  const mempa::DemHandler* demHandler) {
  this->averageSlope = 0.0;
  
  if (!demHandler || path.size() < 2) {
    return;
  }
  
  float totalSlope = 0.0;
  int validSegments = 0;
  
  for (size_t i = 1; i < path.size(); i++) {
    std::pair<int, int> before = path[i - 1];
    std::pair<int, int> after = path[i];
    
    // Calculate horizontal distance
    float horizontalDist = std::sqrt(
        std::pow(after.first - before.first, 2) +
        std::pow(after.second - before.second, 2));
    
    // Skip calculations for very small horizontal distances
    if (horizontalDist < 0.001) {
      continue;
    }
    
    // Get elevation data
    float elevBefore = 0.0f;
    float elevAfter = 0.0f;
    
    try {
      elevBefore = demHandler->getValue(before.first, before.second);
      elevAfter = demHandler->getValue(after.first, after.second);
      
      float elevDiff = std::abs(elevAfter - elevBefore);
      
      // Calculate slope in degrees
      float segmentSlope = std::atan(elevDiff / horizontalDist) * (180.0 / M_PI);
      totalSlope += segmentSlope;
      validSegments++;
    } catch (const std::exception& e) {
      continue;
    }
  }
  
  // Calculate average if we have valid segments
  if (validSegments > 0) {
    this->averageSlope = totalSlope / validSegments;
    std::cout << "Average slope: " << this->averageSlope << " degrees " 
              << "(from " << validSegments << " segments)" << std::endl;
  } else {
    std::cout << "No valid segments for average slope calculation" << std::endl;
  }
}

void Metrics::analyzePath(const std::vector<std::pair<int, int>>& path, 
  const mempa::DemHandler* demHandler) {
this->reset();

std::cout << "\n===== METRICS CALCULATION START =====" << std::endl;
std::cout << "Path has " << path.size() << " points" << std::endl;
std::cout << "DEM handler address: " << (void*)demHandler << std::endl;

// Calculate basic metrics directly
this->analizePath(path);

// Then add the elevation-based metrics if a DEM handler is available
if (demHandler) {
std::cout << "DEM handler is provided" << std::endl;

// Test if the DEM handler is actually producing elevation data
try {
float startElev = 0.0f;
if (path.size() > 0) {
startElev = demHandler->getValue(path[0].first, path[0].second);
std::cout << "Start point elevation: " << startElev << std::endl;

// Try calculating all metrics
this->calculateTotalDistance(path, demHandler);
this->calculateTotalElevationChange(path, demHandler);
this->calculateNetElevationChange(path, demHandler);
this->calculateMaxSlope(path, demHandler);
this->calculateAverageSlope(path, demHandler);
}
} catch (const std::exception& e) {
std::cerr << "ERROR: Failed to use DEM handler: " << e.what() << std::endl;
}
} else {
std::cout << "WARNING: No DEM handler provided - using 2D metrics only" << std::endl;
}

std::cout << "Final metrics values:" << std::endl;
std::cout << "  horizontalDistance: " << this->horizontalDistance << std::endl;
std::cout << "  asTheCrowFlysDistance: " << this->asTheCrowFlysDistance << std::endl;
std::cout << "  totalDistance: " << this->totalDistance << std::endl;
std::cout << "  totalElevationChange: " << this->totalElevationChange << std::endl;
std::cout << "  netElevationChange: " << this->netElevationChange << std::endl;
std::cout << "  maxSlope: " << this->maxSlope << std::endl;
std::cout << "  averageSlope: " << this->averageSlope << std::endl;
std::cout << "===== METRICS CALCULATION END =====" << std::endl;
}
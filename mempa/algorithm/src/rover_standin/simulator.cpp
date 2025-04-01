#include "../DemHandler/DemHandler.h"
#include "../search_algorithms/dijkstras.h"
#include "../search_algorithms/SearchContext.h"
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
#include <unistd.h>
#include <vector>
#include <tuple>
#include <typeinfo>

// Constants for Mars terrain analysis
const float MARS_PIXEL_SIZE = 200.0; // Mars DEM resolution (meters)
const float MAX_SLOPE = 25.0;        // Maximum allowed slope (degrees)
const float MM_TO_M = 0.001;         // Millimeter to meter conversion
const uint32_t TERRAIN_PADDING = 15;  // Padding for path finding
const float MAX_ELEVATION_CHANGE =
    50.0; // Maximum elevation change per pixel (meters)
const float ELEVATION_EPSILON =
    0.1; // Small elevation difference threshold (meters)
const float MIN_MARS_ELEVATION = -8500.0; // Minimum Mars elevation (meters)
const float MAX_MARS_ELEVATION = 21230.0; // Maximum Mars elevation (meters)
const float ELEVATION_SCALING = 0.001;    // Scale factor for large elevations
const float SIGNIFICANT_CHANGE =
    10.0; // Significant elevation change threshold (meters)

/**
 * @brief Class to hold Terrain Metrics to display information discorved about the path to the user
 *
 * @author Adam Carlson
 */
class TerrainMetrics {
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
 * @brief Resets all of the TerrianMetric class values to their default values. 
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
 * @brief A check to see if the TerrianMetrics calss has been initialized
 *
 * @author Adam Carlson
 */
  bool isInitialized() const {
    return !relativeHeights.empty() && !relativeHeights[0].empty();
  }

  /**
 * @brief calculates a value to represent the energy cost of a path.  Takes into account distaance and elavation change
 *
 * @author Adam Carlson
 */
  float calculateEnergyCost(float distance, float elevChange, float slope) {
    float slopePenalty = std::pow(slope / MAX_SLOPE, 2);
    float elevationPenalty = std::abs(elevChange) * 2.0;
    return distance + elevationPenalty + (distance * slopePenalty);
  }
};

/**
 * @brief Class to track the progress of the pathfinding
 *
 * @author Adam Carlson
 */
class ProgressTracker {
private:
  const char *spinner = "⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏";
  int spinnerIndex = 0;

public:
  /**
 * @brief function to change the displyed progress spinner
 *
 * @author Adam Carlson
 */
  void updateProgress(const std::string &status, float progress) {
    std::cout << "\r" << spinner[spinnerIndex] << " " << status << ": "
              << std::fixed << std::setprecision(1) << progress << "%"
              << std::flush;
    spinnerIndex = (spinnerIndex + 1) % 10;
  }

  /**
 * @brief function to show that the search is complete.  
 *
 * @author Adam Carlson
 */
  void complete(const std::string &message) {
    std::cout << "\r✓ " << message << std::endl;
  }
};

/**
 * @brief The Simulator class, this class is used to abstract the starting and goal conditions from the DEM files and search algoritm used to traverse the DEM file
 *
 * @author Adam Carlson and Oscar Mikus
 */
class Simulator {
private:
  TerrainMetrics metrics;
  std::string name;
  std::chrono::steady_clock::time_point startTime;
  ProgressTracker progress;
  std::pair<int,int> currentPos;

  /**
 * @brief Used to output debug messages within the Simulator class
 *
 * @author Adam Carlson
 */
  void logDebug(const std::string &message) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime)
            .count();
    std::cout << "[DEBUG:" << elapsed << "ms] " << message << std::endl;
  }

  /**
 * @brief Given a cordinate in std::pair<int, int> and a heightmap std::vector<std::vector<float>> retuns a truth value based on if the coodinate pair is within the heightmap.  
 *
 * @author Adam Carlson
 */
  bool isValidCoordinate(const std::pair<int, int> &coord,
                         const std::vector<std::vector<float>> &heightmap) {
    if (!metrics.isInitialized()) {
      throw std::runtime_error("Terrain metrics not initialized");
    }

    if (heightmap.empty() || heightmap[0].empty()) {
      throw std::runtime_error("Invalid heightmap");
    }

    bool valid = coord.first >= 0 &&
                 coord.first < static_cast<int>(heightmap.size()) &&
                 coord.second >= 0 &&
                 coord.second < static_cast<int>(heightmap[0].size());

    if (!valid) {
      logDebug("Invalid coordinate (" + std::to_string(coord.first) + "," +
               std::to_string(coord.second) + ") heightmap.size(): " + std::to_string(heightmap.size()) + " heightmap[0].size(): " + std::to_string(heightmap[0].size()));
    }
    return valid;
  }

  /**
   * @brief retruns a height value for elavation or 0.0 if a height value of Nan is found.  
   *
   * @author Adam Carlson
   */
  float validateElevation(float rawElevation) {
    if (std::isnan(rawElevation)) {
      logDebug("Warning: NaN elevation detected");
      return 0.0;
    }

    float elevation = rawElevation * MM_TO_M;
    if (elevation > 1000000.0) {
      elevation *= ELEVATION_SCALING;
      logDebug("Scaled large elevation from " +
               std::to_string(elevation / ELEVATION_SCALING) + "m to " +
               std::to_string(elevation) + "m");
    }

    return elevation;
  }

/**
 * @brief Converts raw elevation values into normalized relative elevations
 *
 * This function processes raw elevation data (in millimeters) to prepare it for path analysis by:
 * 1. Validating input coordinates are within terrain bounds
 * 2. Converting raw values from millimeters to meters using validateElevation()
 * 3. Establishing a base elevation reference point (using the first elevation value processed)
 * 4. Calculating relative elevations as the difference from this base elevation
 * 5. Storing these relative values in the metrics matrix for later calculations
 * 
 * Using relative elevations rather than absolute values simplifies slope and energy
 * consumption calculations, as the rover's performance is affected by changes in
 * elevation rather than the absolute height above Mars datum.
 *
 * @param rawElevation Raw elevation value in millimeters from the DEM file
 * @param row Row index in the heightmap matrix
 * @param col Column index in the heightmap matrix
 * @return The processed relative elevation in meters
 * @author Adam Carlson
 */
  float processElevation(float rawElevation, int row, int col) {
    if (!metrics.isInitialized()) {
      throw std::runtime_error("Terrain metrics not initialized");
    }

    if (row < 0 || row >= static_cast<int>(metrics.relativeHeights.size()) ||
        col < 0 || col >= static_cast<int>(metrics.relativeHeights[0].size())) {
      throw std::runtime_error("Elevation coordinates out of bounds");
    }

    float elevation = validateElevation(rawElevation);

    if (!metrics.baseElevationSet) {
      metrics.baseElevation = elevation;
      metrics.baseElevationSet = true;
      logDebug("Base elevation set to: " +
               std::to_string(metrics.baseElevation) + "m");
      return 0.0;
    }

    float relativeElevation = elevation - metrics.baseElevation;
    metrics.relativeHeights[row][col] = relativeElevation;

    static int warningCount = 0;
    if (warningCount++ < 5) {
      std::stringstream ss;
      ss << std::fixed << std::setprecision(2) << "Raw=" << rawElevation
         << "mm, "
         << "Abs=" << elevation << "m, "
         << "Rel=" << relativeElevation << "m";
      logDebug(ss.str());
    }

    return relativeElevation;
  }

  /**
 * @brief used to calculate the slope based on the change in x and y coordinates and an absolute elavation change
 *
 * @author Adam Carlson
 */
  float calculateSlope(float dx, float dy, float elevChange) {
    float horizontalDist = std::sqrt(dx * dx + dy * dy) * MARS_PIXEL_SIZE;
    if (horizontalDist < ELEVATION_EPSILON)
      return 0.0;

    elevChange = std::min(std::abs(elevChange), MAX_ELEVATION_CHANGE);
    float slope = std::atan2(elevChange, horizontalDist) * 180.0 / M_PI;
    return std::min(slope, MAX_SLOPE);
  }

  /**
 * @brief Converts pixel coordinates from an entire DEM file to the local pixel coordinates of a chunk of a DEM file
 *
 * @author Adam Carlson
 */
  std::pair<int, int> convertToLocalCoordinates(int globalX, int globalY,
    uint32_t startRow,
    uint32_t startCol) {
    int localX = globalX - startCol;
    int localY = globalY - startRow;
    logDebug("Converting global (" + std::to_string(globalX) + "," +
             std::to_string(globalY) + ") to local (" + std::to_string(localX) +
             "," + std::to_string(localY) + ")");
    return {localX, localY};
  }

  /**
 * @brief Used to change local pixel coordintes of a small DEM chunk back to the pixel coordinates of an entire DEM file
 *
 * @author Oscar Mikus
 */
  std::pair<int, int> convertToGlobalCoordinates(int localX, int localY,
    uint32_t startRow,
    uint32_t startCol) {
    int globalX = localX + startCol;
    int globalY = localY + startRow;
    // logDebug("Converting local (" + std::to_string(localX) + "," +
    //     std::to_string(localY) + ") to global (" + std::to_string(globalX) +
    //     "," + std::to_string(globalY) + ")");
    return {globalX, globalY};
  }

  /**
 * @brief For displaying the path taken in termianl.  Removes many coordinates from the path to not display too many values in terminal.
 *
 * @author Adam Carlson
 */
  void optimizePath(std::vector<std::pair<int, int>> &path) {
    if (path.size() < 3)
      return;

    if (!metrics.isInitialized()) {
      throw std::runtime_error("Terrain metrics not initialized");
    }

    std::vector<std::pair<int, int>> optimized;
    optimized.reserve(path.size());
    optimized.push_back(path.front());

    size_t originalSize = path.size();
    float lastSignificantElev =
        metrics.relativeHeights[path[0].first][path[0].second];

    for (size_t i = 1; i < path.size() - 1; i++) {
      const auto &curr = path[i];
      float currElev = metrics.relativeHeights[curr.first][curr.second];

      if (std::abs(currElev - lastSignificantElev) > SIGNIFICANT_CHANGE) {
        optimized.push_back(curr);
        lastSignificantElev = currElev;
        continue;
      }

      const auto &prev = path[i - 1];
      const auto &next = path[i + 1];
      float dx1 = curr.first - prev.first;
      float dy1 = curr.second - prev.second;
      float dx2 = next.first - curr.first;
      float dy2 = next.second - curr.second;

      float angle = std::abs(std::atan2(dy2, dx2) - std::atan2(dy1, dx1));
      if (angle > M_PI / 4) {
        optimized.push_back(curr);
        continue;
      }

      if (i % 5 == 0) {
        optimized.push_back(curr);
      }
    }

    optimized.push_back(path.back());
    path = std::move(optimized);

    logDebug("Path optimized from " + std::to_string(originalSize) + " to " +
             std::to_string(path.size()) + " waypoints");
  }

/**
 * @brief Calculates comprehensive terrain metrics for a rover path
 *
 * This function analyzes a path through the Mars terrain by:
 * 1. Initializing the metrics data structure and validating all path coordinates
 * 2. Processing elevations at start and end points to establish net elevation change
 * 3. Iterating through path segments to calculate per-segment and cumulative metrics:
 *    - Horizontal and 3D distances
 *    - Elevation changes between waypoints
 *    - Slope angles and their statistics
 *    - Energy cost estimates based on distance, slope and elevation
 * 4. Aggregating data for overall path assessment (total distance, max slope, etc.)
 * 
 * These metrics are essential for evaluating path quality, rover capability 
 * requirements, and energy consumption estimates for the planned traverse.
 *
 * @param path Vector of coordinate pairs representing waypoints in the rover's path
 * @param heightmap 2D vector containing elevation data for the terrain
 * @throws std::runtime_error If path is empty or contains invalid coordinates
 * @author Adam Carlson
 */
  void updatePathMetrics(const std::vector<std::pair<int, int>> &path,
    const std::vector<std::vector<float>> &heightmap) {
    if (path.empty()) {
      throw std::runtime_error("Empty path provided");
    }

    try {
      metrics.initialize(heightmap.size(), heightmap[0].size());
      metrics.waypointCount = path.size();

      for (const auto &point : path) {
        if (!isValidCoordinate(point, heightmap)) {
          throw std::runtime_error("Invalid coordinate in path");
        }
      }

      float startElev =
          processElevation(heightmap[path[0].first][path[0].second],
          path[0].first, path[0].second);
      float endElev =
          processElevation(heightmap[path.back().first][path.back().second],
          path.back().first, path.back().second);
      metrics.netElevationChange = endElev - startElev;

      for (size_t i = 1; i < path.size(); ++i) {
        const auto &curr = path[i];
        const auto &prev = path[i - 1];

        float dx = curr.first - prev.first;
        float dy = curr.second - prev.second;
        float horizontalDist = std::sqrt(dx * dx + dy * dy) * MARS_PIXEL_SIZE;

        float elev1 = metrics.relativeHeights[prev.first][prev.second];
        float elev2 = metrics.relativeHeights[curr.first][curr.second];
        float elevChange = std::abs(elev2 - elev1);

        float slope = calculateSlope(dx, dy, elevChange);
        float distance = std::sqrt(horizontalDist * horizontalDist +
        elevChange * elevChange);

        metrics.totalDistance += distance;
        metrics.horizontalDistance += horizontalDist;
        metrics.totalElevationChange += elevChange;
        metrics.maxSlope = std::max(metrics.maxSlope, slope);
        metrics.slopes.push_back(slope);
        metrics.energyCost +=
            metrics.calculateEnergyCost(distance, elevChange, slope);

        if (i % 10 == 0 || i == path.size() - 1) {
          std::stringstream ss;
          ss << std::fixed << std::setprecision(2) << "Step " << i << "/"
             << (path.size() - 1) << ": dist=" << distance << "m"
             << ", horiz=" << horizontalDist << "m"
             << ", elev=" << elevChange << "m"
             << ", slope=" << slope << "°"
             << ", energy=" << metrics.energyCost;
          logDebug(ss.str());
        }
      }

      if (!metrics.slopes.empty()) {
        metrics.averageSlope =
            std::accumulate(metrics.slopes.begin(), metrics.slopes.end(), 0.0) /
            metrics.slopes.size();
      }
    } catch (const std::exception &e) {
      logDebug("Error in updatePathMetrics: " + std::string(e.what()));
      throw;
    }
  }

/**
 * @brief Exports path data and metrics to a formatted text file
 *
 * This function creates a detailed report file containing:
 * 1. Path metadata (start/end points, timestamp)
 * 2. Comprehensive terrain metrics (distances, elevations, slopes, energy cost)
 * 3. Waypoint coordinates with associated elevations
 * 
 * The output file uses a structured format with comments (prefixed by '#') 
 * for metadata and a simple space-delimited format for coordinate data.
 * All coordinates are converted from local heightmap indices to global 
 * DEM coordinates before writing to ensure consistency across analyses.
 *
 * @param filename Output file path where data will be written
 * @param path Vector of coordinate pairs representing the rover's path
 * @param heightmap 2D vector containing elevation data for the terrain
 * @param startX Global X-coordinate of the path's starting point
 * @param startY Global Y-coordinate of the path's starting point
 * @param endX Global X-coordinate of the path's destination
 * @param endY Global Y-coordinate of the path's destination
 * @param startRow Row offset for converting local to global coordinates
 * @param startCol Column offset for converting local to global coordinates
 * @throws std::runtime_error If file cannot be created
 * @author Adam Carlson
 */
  void writePathData(const std::string &filename,
                     const std::vector<std::pair<int, int>> &path,
                     const std::vector<std::vector<float>> &heightmap,
                     int startX, int startY, int endX, int endY,
                     uint32_t startRow, uint32_t startCol) {
    std::ofstream outFile(filename);
    if (!outFile) {
      throw std::runtime_error("Failed to create output file: " + filename);
    }

    outFile << std::fixed << std::setprecision(3);
    outFile << "# Mars Rover Path Analysis\n";
    outFile << "# Generated: " << std::time(nullptr) << "\n";
    outFile << "# Start: (" << startX << "," << startY << ")\n";
    outFile << "# End: (" << endX << "," << endY << ")\n";
    outFile << "# Metrics:\n";
    outFile << "#   Total Distance: " << metrics.totalDistance << " m\n";
    outFile << "#   Horizontal Distance: " << metrics.horizontalDistance
            << " m\n";
    outFile << "#   Total Elevation Change: " << metrics.totalElevationChange
            << " m\n";
    outFile << "#   Net Elevation Change: " << metrics.netElevationChange
            << " m\n";
    outFile << "#   Maximum Slope: " << metrics.maxSlope << "°\n";
    outFile << "#   Average Slope: " << metrics.averageSlope << "°\n";
    outFile << "#   Energy Cost: " << metrics.energyCost << "\n";
    outFile << "#   Base Elevation: " << metrics.baseElevation << " m\n";
    outFile << "#   Waypoints: " << metrics.waypointCount << "\n\n";
    outFile << "# X Y Elevation(m) \n";

    for (size_t i = 0; i < path.size(); ++i) {
      const auto &coord = path[i];
      float elevation = heightmap[coord.first][coord.second] * MM_TO_M;

      // Fix coordinate ordering: swap row/col to match X/Y convention
      int globalY = coord.first + startRow;
      int globalX = coord.second + startCol;

      outFile << globalX << " " << globalY << " " << elevation << " "
              << "\n";
    }

    outFile.close();
    logDebug("Path data written to " + filename);
  }

/**
 * @brief Displays advanced performance metrics about the rover path
 *
 * This function calculates and presents higher-level analytics including:
 * 1. Path efficiency - Ratio of direct distance to actual traversed path length
 *    (lower values indicate more detours taken to avoid obstacles)
 * 2. Processing rate - Algorithm performance measured in meters processed per second
 * 3. Energy per meter - Energy consumption efficiency along the path
 * 
 * These metrics help evaluate the quality and efficiency of the generated path
 * beyond simple distance measurements, providing insights into the algorithm's 
 * performance characteristics and energy optimization capabilities.
 *
 * @param startX Global X-coordinate of the path's starting point
 * @param startY Global Y-coordinate of the path's starting point
 * @param endX Global X-coordinate of the path's destination
 * @param endY Global Y-coordinate of the path's destination
 * @author Adam Carlson
 */
  void displayDetailedSummary(int startX, int startY, int endX, int endY) {
    const float metersPerPixel = MARS_PIXEL_SIZE;
    const float actualDistance =
        std::sqrt(std::pow((endX - startX) * metersPerPixel, 2) +
                  std::pow((endY - startY) * metersPerPixel, 2));

    auto now = std::chrono::steady_clock::now();
    auto execution_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime)
            .count();

    std::cout << "\nDetailed Analysis:\n"
              << "----------------\n"
              << std::fixed << std::setprecision(1) << "Path Efficiency: "
              << (actualDistance / metrics.totalDistance * 100.0) << "%\n"
              << "Processing Rate: "
              << (metrics.totalDistance / (execution_time / 1000.0)) << " m/s\n"
              << "Energy per Meter: "
              << (metrics.energyCost / metrics.totalDistance) << "\n";
  }

  void displaySimulationHeader(int startX, int startY, int endX, int endY) {
    std::cout << "\n=================================\n"
              << "NASA JPL Mars Rover Standin\n"
              << "=================================\n\n"
              << "Configuration:\n"
              << "- Start: (" << startX << ", " << startY << ")\n"
              << "- End:   (" << endX << ", " << endY << ")\n"
              << "- Resolution: " << MARS_PIXEL_SIZE << " m/pixel\n"
              << "- Max slope: " << MAX_SLOPE << "°\n\n";
  }

  /**
 * @brief updates the ProgressTracker inside of the Simulator class with new information
 *
 * @author Adam Carlson
 */
  void logProgress(const std::string &status, int current, int total) {
    float percent = (current * 100.0) / total;
    progress.updateProgress(status, percent);
  }

/**
 * @brief Displays formatted path analysis results in the terminal
 *
 * This function presents a comprehensive summary of rover path metrics
 * in a user-friendly, formatted display including:
 * - Path distance measurements (3D and 2D)
 * - Elevation statistics (total change, net change)
 * - Slope information (maximum and average angles)
 * - Energy consumption estimate
 * - Path composition data (base elevation, waypoint count)
 * 
 * The output is organized into clear sections with appropriate headers
 * and consistent decimal precision for readability. It also indicates
 * where detailed path data has been stored for further analysis.
 *
 * @author Adam Carlson
 */
  void displayResults() {
    std::cout << "\nSimulation Results:\n"
              << "==================\n\n"
              << "Path Metrics:\n"
              << "-----------\n"
              << std::fixed << std::setprecision(2)
              << "3D Distance: " << metrics.totalDistance << " m\n"
              << "2D Distance: " << metrics.horizontalDistance << " m\n"
              << "Total Elevation Change: " << metrics.totalElevationChange
              << " m\n"
              << "Net Elevation Change: " << metrics.netElevationChange
              << " m\n"
              << "Maximum Slope: " << metrics.maxSlope << "°\n"
              << "Average Slope: " << metrics.averageSlope << "°\n"
              << "Energy Cost: " << metrics.energyCost << "\n"
              << "Base Elevation: " << metrics.baseElevation << " m\n"
              << "Waypoints: " << metrics.waypointCount << "\n"
              << "\nPerformance Metrics:\n"
              << "-----------------\n"
              << "\nOutput:\n"
              << "-------\n"
              << "- Detailed path data written to: rover_path.txt\n";
  }

  /**
 * @brief calculates the coordiantes needed to create a region by the DEM handeler function
 *
 * @author Oscar Mikus, Adam Carlson
 */
  std::tuple<int, int, int, int> calculateRegionCordinates(int startX, int startY, int endX, int endY)
  {
    int startRow =
          (startY > TERRAIN_PADDING) ? startY - TERRAIN_PADDING : 0;
      int endRow =
          std::min(static_cast<int>(endY + TERRAIN_PADDING), 53347);  //TODO make 53347u a parameter of some sort
      int startCol =
          (startX > TERRAIN_PADDING) ? startX - TERRAIN_PADDING : 0;
      int endCol =
          std::min(static_cast<int>(endX + TERRAIN_PADDING), 106694); //TODO make 106694u a parameter of some sort
    
    std::cout << startRow << " " << startCol << " " << endRow << " " << endCol << std::endl;
    return std::make_tuple(startRow, startCol, endRow, endCol);
  }

  /**
 * @brief this function caluculates the closest cordinate inside of a square area to an end point
 *
 * @author Oscar Mikus
 */
  std::pair<int, int> calculateDemNavToHereRadius(int startX, int startY, int endX, int endY, int radius)
  {
    int left = startX - radius;
    int right = startX + radius;
    int bottom = startY - radius;
    int top = startY + radius;

    int closestX = std::clamp(endX, left, right);
    int closestY = std::clamp(endY, bottom, top);

    return std::make_pair(closestX, closestY);
  }

  
  /**
 * @brief this function calulates the cordinates for the closesst postion to the actual goal cordinate within the limitations of the max dem size based on a max number of pixels to represent memory limitations
 *
 * @author Oscar Mikus
 */
  std::pair<int, int> calculateDemNavToHere(int startX, int startY, int endX, int endY, int maxDemRegionPixelCount)
  {
    if(maxDemRegionPixelCount < (TERRAIN_PADDING * TERRAIN_PADDING))
    {
      throw std::runtime_error("maxDemRegionPixelCount is too low");
    }

    int width = endX - startX;
    int height = endY - startY;
    int closestX;
    int closestY;

    // Calculate the desired area of the rectangle
    int desiredArea = width * height;

    // If the area exceeds the max pixel limit, adjust the dimensions
    if (desiredArea > maxDemRegionPixelCount) {
        // Try adjusting the width or height while keeping the area under the limit
        float aspectRatio = static_cast<float>(width) / height;

        // Try adjusting the width first
        int newWidth = std::min(width, static_cast<int>(sqrt(maxDemRegionPixelCount * aspectRatio)));
        int newHeight = maxDemRegionPixelCount / newWidth;

        if (newWidth * newHeight > maxDemRegionPixelCount) {
            // If the new width and height do not satisfy the constraint, adjust height
            newHeight = std::min(height, maxDemRegionPixelCount / width);
            newWidth = maxDemRegionPixelCount / newHeight;
        }

        closestX = startX + newWidth;
        closestY = startY + newHeight;
    } else {
        // If the area is within the limit, return the original end point
        closestX = endX;
        closestY = endY;
    }

    return std::make_pair(closestX, closestY);
  }

public:

  /**
 * @brief Defualt Constructor for Simulator class.  Intilize the start time for the simulator
 *
 * @author Adam Carlson
 */
  explicit Simulator(std::string n) : name(std::move(n)) {
    startTime = std::chrono::steady_clock::now();
  }

  /**
 * @brief Main function to call within the Simulator class.  This will navigate from the startpoint to the endpoint in a given DEM file whie adhearing to the max pixel size constraint
 *
 * @author Adam Carlson, Oscar Mikus
 */
std::vector<std::pair<int,int>> runWithSquareRadius(int startX, int startY, int endX, int endY, mempa::DemHandler &demHandler, int radius, SearchAlgorithm* searchStratToUse) {
  displaySimulationHeader(startX, startY, endX, endY);

  /*const std::string demFilePath =
   * "/Users/adamcarlson/Dropbox/Mac/Desktop/Desktop/"*/
  /*                                "nasa-jpl-capstone-2024-2025/prototypes/sim/"*/
  /*                                "Mars_HRSC_MOLA_BlendDEM_Global_200mp_v2.tif";*/
  /*const std::string outputPath =
   * "/Users/adamcarlson/Dropbox/Mac/Downloads";*/

  try {
    logDebug("Initializing simulation");
    //MEMPA::BUFFDEM demHandler(demFilePath, outputPath);
    currentPos.first = startX;
    currentPos.second = startY;

    std::tuple<int, int, int, int> regionCordinates;
    std::pair<int, int> DemNavToHere;
    std::vector<std::pair<int,int>> path;
    std::vector<std::vector<float>> heightmap;
    SearchContext search;

    search.setStrategy(searchStratToUse);

    //start looping here
    while(currentPos.first != endX && currentPos.second != endY)
    {

      // Calculate region with padding
      DemNavToHere = calculateDemNavToHereRadius(currentPos.first, currentPos.second, endX, endY, radius);
      regionCordinates = calculateRegionCordinates(currentPos.first, currentPos.second, DemNavToHere.first, DemNavToHere.second);

      //logDebug("Loading heightmap for sqaure area at (" + currentPos.first + ", " + currentPos.second + ") with radius " + radius);

      heightmap = demHandler.readSquareChunk(currentPos, radius);

      if (heightmap.empty() || heightmap[0].empty()) {
        throw std::runtime_error("Failed to load heightmap data");
      }

      // Initialize metrics before pathfinding
      metrics.initialize(heightmap.size(), heightmap[0].size());

      logDebug("Heightmap loaded: " + std::to_string(heightmap.size()) + "x" +
              std::to_string(heightmap[0].size()));

      auto localStart =
          convertToLocalCoordinates(currentPos.first, currentPos.second, std::get<0>(regionCordinates), std::get<1>(regionCordinates));
      auto localEnd = convertToLocalCoordinates(DemNavToHere.first, DemNavToHere.second, std::get<0>(regionCordinates), std::get<1>(regionCordinates));

      if (!isValidCoordinate(localStart, heightmap) ||
          !isValidCoordinate(localEnd, heightmap)) {
        throw std::runtime_error("Start or end point out of bounds");
      }


      logDebug("Starting to use strategy");
      search.executeStrategyReset();
      logDebug("Did first strategy thing");
      search.executeStrategySetUpAlgo(heightmap, localStart, localEnd, MAX_SLOPE, MARS_PIXEL_SIZE);
      logDebug("Did second strategy thing");

      // auto path = dijkstra.dijkstras(heightmap, localStart, localEnd, MAX_SLOPE,
      //                                MARS_PIXEL_SIZE);

      std::pair<int,int> localStep;
      std::pair<int,int> globalStep;
      while (true)
      {
        localStep = search.executeStrategyGetStep();
        if(localStep == std::make_pair(-1, -1))
        {
          break;
        }
        globalStep = convertToGlobalCoordinates(localStep.first, localStep.second, std::get<0>(regionCordinates), std::get<1>(regionCordinates));
        path.push_back(globalStep);
      }

      //need to set new start cordinates here
      currentPos = globalStep;
    }

    if (path.empty()) {
      throw std::runtime_error("No valid path found between points");
    }

    regionCordinates = calculateRegionCordinates(startX, startY, endX, endY);
    heightmap = demHandler.readRectangleChunk(std::make_pair(std::make_pair(std::get<0>(regionCordinates), std::get<1>(regionCordinates)), std::make_pair(std::get<2>(regionCordinates), std::get<3>(regionCordinates))), 0);

    //update path to local cordinates in this final heightmap
    for(std::pair<int,int>& step : path)
    {
      step = convertToLocalCoordinates(step.first, step.second, std::get<0>(regionCordinates), std::get<1>(regionCordinates));
    }

    updatePathMetrics(path, heightmap);
    optimizePath(path);
    writePathData("rover_path.txt", path, heightmap, startX, startY, endX,
                  endY, std::get<0>(regionCordinates), std::get<1>(regionCordinates));

    // Display summary
    std::cout << "\nPath Summary:\n";
    std::cout << "------------\n";
    std::cout << std::fixed << std::setprecision(2)
              << "3D Distance: " << metrics.totalDistance << " m\n"
              << "2D Distance: " << metrics.horizontalDistance << " m\n"
              << "Total Elevation Change: " << metrics.totalElevationChange
              << " m\n"
              << "Net Elevation Change: " << metrics.netElevationChange
              << " m\n"
              << "Maximum Slope: " << metrics.maxSlope << "°\n"
              << "Average Slope: " << metrics.averageSlope << "°\n"
              << "Energy Cost: " << metrics.energyCost << "\n"
              << "Base Elevation: " << metrics.baseElevation << " m\n"
              << "Waypoints: " << metrics.waypointCount << "\n"
              << "Execution Time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::steady_clock::now() - startTime)
                     .count()
              << "ms\n";
    displayDetailedSummary(startX, startY, endX, endY);

    return path;
  } catch (const std::exception &e) {
    logDebug("Error in run: " + std::string(e.what()));
    throw;
  }
  }


  /**
 * @brief Main function to call within the Simulator class.  This will navigate from the startpoint to the endpoint in a given DEM file whie adhearing to the max pixel size constraint
 *
 * @author Adam Carlson, Oscar Mikus
 */
  std::vector<std::pair<int,int>> run(int startX, int startY, int endX, int endY, mempa::DemHandler &demHandler, int maxDemRegionPixelCount, SearchAlgorithm* searchStratToUse) {
    displaySimulationHeader(startX, startY, endX, endY);

    /*const std::string demFilePath =
     * "/Users/adamcarlson/Dropbox/Mac/Desktop/Desktop/"*/
    /*                                "nasa-jpl-capstone-2024-2025/prototypes/sim/"*/
    /*                                "Mars_HRSC_MOLA_BlendDEM_Global_200mp_v2.tif";*/
    /*const std::string outputPath =
     * "/Users/adamcarlson/Dropbox/Mac/Downloads";*/

    try {
      logDebug("Initializing simulation");
      //MEMPA::BUFFDEM demHandler(demFilePath, outputPath);
      currentPos.first = startX;
      currentPos.second = startY;

      std::tuple<int, int, int, int> regionCordinates;
      std::pair<int, int> DemNavToHere;
      std::vector<std::pair<int,int>> path;
      std::vector<std::vector<float>> heightmap;
      SearchContext search;

      search.setStrategy(searchStratToUse);

      //start looping here
      while(currentPos.first != endX && currentPos.second != endY)
      {

        // Calculate region with padding
        DemNavToHere = calculateDemNavToHere(currentPos.first, currentPos.second, endX, endY, maxDemRegionPixelCount);
        regionCordinates = calculateRegionCordinates(currentPos.first, currentPos.second, DemNavToHere.first, DemNavToHere.second);

        logDebug("Loading heightmap for region: (" + std::to_string(std::get<0>(regionCordinates)) +
                "," + std::to_string(std::get<1>(regionCordinates)) + ") to (" +
                std::to_string(std::get<2>(regionCordinates)) + "," + std::to_string(std::get<3>(regionCordinates)) + ")");

        heightmap = demHandler.readRectangleChunk(std::make_pair(std::make_pair(std::get<0>(regionCordinates), std::get<1>(regionCordinates)), std::make_pair(std::get<2>(regionCordinates), std::get<3>(regionCordinates))), 0);

        if (heightmap.empty() || heightmap[0].empty()) {
          throw std::runtime_error("Failed to load heightmap data");
        }

        // Initialize metrics before pathfinding
        metrics.initialize(heightmap.size(), heightmap[0].size());

        logDebug("Heightmap loaded: " + std::to_string(heightmap.size()) + "x" +
                std::to_string(heightmap[0].size()));

        auto localStart =
            convertToLocalCoordinates(currentPos.first, currentPos.second, std::get<0>(regionCordinates), std::get<1>(regionCordinates));
        auto localEnd = convertToLocalCoordinates(DemNavToHere.first, DemNavToHere.second, std::get<0>(regionCordinates), std::get<1>(regionCordinates));

        if (!isValidCoordinate(localStart, heightmap) ||
            !isValidCoordinate(localEnd, heightmap)) {
          throw std::runtime_error("Start or end point out of bounds");
        }


        logDebug("Starting to use strategy");
        search.executeStrategyReset();
        logDebug("Did first strategy thing");
        search.executeStrategySetUpAlgo(heightmap, localStart, localEnd, MAX_SLOPE, MARS_PIXEL_SIZE);
        logDebug("Did second strategy thing");

        // auto path = dijkstra.dijkstras(heightmap, localStart, localEnd, MAX_SLOPE,
        //                                MARS_PIXEL_SIZE);

        std::pair<int,int> localStep;
        std::pair<int,int> globalStep;
        while (true)
        {
          localStep = search.executeStrategyGetStep();
          if(localStep == std::make_pair(-1, -1))
          {
            break;
          }
          globalStep = convertToGlobalCoordinates(localStep.first, localStep.second, std::get<0>(regionCordinates), std::get<1>(regionCordinates));
          path.push_back(globalStep);
        }

        //need to set new start cordinates here
        currentPos = globalStep;
      }

      if (path.empty()) {
        throw std::runtime_error("No valid path found between points");
      }

      regionCordinates = calculateRegionCordinates(startX, startY, endX, endY);
      heightmap = demHandler.readRectangleChunk(std::make_pair(std::make_pair(std::get<0>(regionCordinates), std::get<1>(regionCordinates)), std::make_pair(std::get<2>(regionCordinates), std::get<3>(regionCordinates))), 0);

      //update path to local cordinates in this final heightmap
      for(std::pair<int,int>& step : path)
      {
        step = convertToLocalCoordinates(step.first, step.second, std::get<0>(regionCordinates), std::get<1>(regionCordinates));
      }

      updatePathMetrics(path, heightmap);
      optimizePath(path);
      writePathData("rover_path.txt", path, heightmap, startX, startY, endX,
                    endY, std::get<0>(regionCordinates), std::get<1>(regionCordinates));

      // Display summary
      std::cout << "\nPath Summary:\n";
      std::cout << "------------\n";
      std::cout << std::fixed << std::setprecision(2)
                << "3D Distance: " << metrics.totalDistance << " m\n"
                << "2D Distance: " << metrics.horizontalDistance << " m\n"
                << "Total Elevation Change: " << metrics.totalElevationChange
                << " m\n"
                << "Net Elevation Change: " << metrics.netElevationChange
                << " m\n"
                << "Maximum Slope: " << metrics.maxSlope << "°\n"
                << "Average Slope: " << metrics.averageSlope << "°\n"
                << "Energy Cost: " << metrics.energyCost << "\n"
                << "Base Elevation: " << metrics.baseElevation << " m\n"
                << "Waypoints: " << metrics.waypointCount << "\n"
                << "Execution Time: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now() - startTime)
                       .count()
                << "ms\n";
      displayDetailedSummary(startX, startY, endX, endY);

      return path;
    } catch (const std::exception &e) {
      logDebug("Error in run: " + std::string(e.what()));
      throw;
    }
  }
};


//current way to run and test the Simulator.  
// int main(int argc, char **argv) {
//   // take argus for in an out fixed
//   std::string inputFile;
//   int startX = 1254;
//   int startY = 1265;
//   int endX = 1340;
//   int endY = 1338;
//   int opt;

//   while ((opt = getopt(argc, argv, "i:x:y:X:Y:")) != -1) {
//     switch (opt) {
//     case 'i':
//       inputFile = optarg;
//       break;
//     case 'x':
//       startX = std::atoi(optarg);
//       break;
//     case 'y':
//       startY = std::atoi(optarg);
//       break;
//     case 'X':
//       endX = std::atoi(optarg);
//       break;
//     case 'Y':
//       endY = std::atoi(optarg);
//       break;
//     case '?':
//       std::cerr
//           << "Usage: " << argv[0]
//           << " -i <input_file> -x <startX> -y <startY> -X <endX> -Y <endY>\n";
//       return 1;
//     }
//   }

//   try {
//     // // Default coordinates for testing
//     // int startX = 1254;
//     // int startY = 1265;
//     // int endX = 1340;
//     // int endY = 1338;

//     // if (argc >= 5) {
//     //   // Override defaults with command line arguments
//     //   startX = std::stoi(argv[1]);
//     //   startY = std::stoi(argv[2]);
//     //   endX = std::stoi(argv[3]);
//     //   endY = std::stoi(argv[4]);
//     // } else if (argc > 1) {
//     //   // If some but not all coordinates provided, show usage
//     //   std::cerr << "Usage: " << argv[0] << " <startX> <startY> <endX>
//     //   <endY>\n"
//     //             << "Valid ranges:\n"
//     //             << "  X: 0 to 106,694\n"
//     //             << "  Y: 0 to 53,347\n"
//     //             << "\nOr run without arguments to use default coordinates:\n"
//     //             << "  Start: (" << startX << "," << startY << ")\n"
//     //             << "  End:   (" << endX << "," << endY << ")\n";
//     //   return 1;
//     // }

//     // Validate input coordinates
//     if (startX < 0 || startY < 0 || endX < 0 || endY < 0 || startX > 106694 ||
//         endX > 106694 || startY > 53347 || endY > 53347) {
//       throw std::runtime_error("Coordinates out of bounds for Mars DEM");
//     }

//     std::cout << "NASA JPL Rover Proxy" << std::endl;
//     std::cout << "Input coordinates: (" << startX << "," << startY << ") to ("
//               << endX << "," << endY << ")" << std::endl;

//     Simulator sim("NASA JPL Rover Simulator");

//     mempa::DemHandler demHandler = mempa::DemHandler(inputFile.c_str());
//     Dijkstras searchAlgoritm;
    
//     sim.runWithSquareRadius(startX, startY, endX, endY, demHandler, 20, &searchAlgoritm);
//     return 0;

//   } catch (const std::exception &e) {
//     std::cerr << "\n Fatal error: " << e.what() << std::endl;
//     return 1;
//   }
// }
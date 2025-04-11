#include "TerrainMetrics.cpp"

class Analyitics {

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
private:
  TerrainMetrics metrics;
  std::string name;
  std::chrono::steady_clock::time_point startTime;
  std::pair<int, int> currentPos;

public:
  /**
   * @brief used to calculate the slope based on the change in x and y
   * coordinates and an absolute elevation change
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
   * @brief Calculates comprehensive terrain metrics for a rover path
   *
   * This function analyzes a path through the Mars terrain by:
   * 1. Initializing the metrics data structure and validating all path
   * coordinates
   * 2. Processing elevations at start and end points to establish net elevation
   * change
   * 3. Iterating through path segments to calculate per-segment and cumulative
   * metrics:
   *    - Horizontal and 3D distances
   *    - Elevation changes between waypoints
   *    - Slope angles and their statistics
   *    - Energy cost estimates based on distance, slope and elevation
   * 4. Aggregating data for overall path assessment (total distance, max slope,
   * etc.)
   *
   * These metrics are essential for evaluating path quality, rover capability
   * requirements, and energy consumption estimates for the planned traverse.
   *
   * @param path Vector of coordinate pairs representing waypoints in the
   * rover's path
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

      // for (const auto &point : path) {
      //   if (!isValidCoordinate(point, heightmap)) {
      //     throw std::runtime_error("Invalid coordinate in path");
      //   }
      // }

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
   * @brief Exports path data and metrics to a formatted text file
   *
   * This function creates a detailed report file containing:
   * 1. Path metadata (start/end points, timestamp)
   * 2. Comprehensive terrain metrics (distances, elevations, slopes, energy
   * cost)
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
   * 1. Path efficiency - Ratio of direct distance to actual traversed path
   * length (lower values indicate more detours taken to avoid obstacles)
   * 2. Processing rate - Algorithm performance measured in meters processed per
   * second
   * 3. Energy per meter - Energy consumption efficiency along the path
   *
   * These metrics help evaluate the quality and efficiency of the generated
   * path beyond simple distance measurements, providing insights into the
   * algorithm's performance characteristics and energy optimization
   * capabilities.
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

  /**
   * @brief Converts raw elevation values into normalized relative elevations
   *
   * This function processes raw elevation data (in millimeters) to prepare it
   * for path analysis by:
   * 1. Validating input coordinates are within terrain bounds
   * 2. Converting raw values from millimeters to meters using
   * validateElevation()
   * 3. Establishing a base elevation reference point (using the first elevation
   * value processed)
   * 4. Calculating relative elevations as the difference from this base
   * elevation
   * 5. Storing these relative values in the metrics matrix for later
   * calculations
   *
   * Using relative elevations rather than absolute values simplifies slope and
   * energy consumption calculations, as the rover's performance is affected by
   * changes in elevation rather than the absolute height above Mars datum.
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
   * @brief For displaying the path taken in terminal. Removes many coordinates
   * from the path to not display too many values in terminal.
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
   * @brief retruns a height value for elevation or 0.0 if a height value of Nan
   * is found.
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
};

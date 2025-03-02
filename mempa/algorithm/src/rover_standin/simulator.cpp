
#include "../dem_handler/buf_dem.h"
#include "../search_algorithms/dijkstras.h"
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

// Constants for Mars terrain analysis
const double MARS_PIXEL_SIZE = 200.0; // Mars DEM resolution (meters)
const double MAX_SLOPE = 25.0;        // Maximum allowed slope (degrees)
const double MM_TO_M = 0.001;         // Millimeter to meter conversion
const uint32_t TERRAIN_PADDING = 15;  // Padding for path finding
const double MAX_ELEVATION_CHANGE =
    50.0; // Maximum elevation change per pixel (meters)
const double ELEVATION_EPSILON =
    0.1; // Small elevation difference threshold (meters)
const double MIN_MARS_ELEVATION = -8500.0; // Minimum Mars elevation (meters)
const double MAX_MARS_ELEVATION = 21230.0; // Maximum Mars elevation (meters)
const double ELEVATION_SCALING = 0.001;    // Scale factor for large elevations
const double SIGNIFICANT_CHANGE =
    10.0; // Significant elevation change threshold (meters)

class TerrainMetrics {
public:
  double totalDistance = 0.0;
  double horizontalDistance = 0.0;
  double totalElevationChange = 0.0;
  double netElevationChange = 0.0;
  double maxSlope = 0.0;
  double averageSlope = 0.0;
  double energyCost = 0.0;
  size_t waypointCount = 0;
  std::vector<double> slopes;
  double baseElevation = 0.0;
  bool baseElevationSet = false;
  std::vector<std::vector<double>> relativeHeights;

  void initialize(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
      throw std::runtime_error("Invalid dimensions for terrain metrics");
    }
    reset();
    relativeHeights.resize(rows, std::vector<double>(cols, 0.0));
  }

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

  bool isInitialized() const {
    return !relativeHeights.empty() && !relativeHeights[0].empty();
  }

  double calculateEnergyCost(double distance, double elevChange, double slope) {
    double slopePenalty = std::pow(slope / MAX_SLOPE, 2);
    double elevationPenalty = std::abs(elevChange) * 2.0;
    return distance + elevationPenalty + (distance * slopePenalty);
  }
};

class ProgressTracker {
private:
  const char *spinner = "⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏";
  int spinnerIndex = 0;

public:
  void updateProgress(const std::string &status, double progress) {
    std::cout << "\r" << spinner[spinnerIndex] << " " << status << ": "
              << std::fixed << std::setprecision(1) << progress << "%"
              << std::flush;
    spinnerIndex = (spinnerIndex + 1) % 10;
  }

  void complete(const std::string &message) {
    std::cout << "\r✓ " << message << std::endl;
  }
};

class Simulator {
private:
  TerrainMetrics metrics;
  std::string name;
  std::chrono::steady_clock::time_point startTime;
  ProgressTracker progress;

  void logDebug(const std::string &message) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime)
            .count();
    std::cout << "[DEBUG:" << elapsed << "ms] " << message << std::endl;
  }

  bool isValidCoordinate(const std::pair<int, int> &coord,
                         const std::vector<std::vector<double>> &heightmap) {
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
               std::to_string(coord.second) + ")");
    }
    return valid;
  }

  double validateElevation(double rawElevation) {
    if (std::isnan(rawElevation)) {
      logDebug("Warning: NaN elevation detected");
      return 0.0;
    }

    double elevation = rawElevation * MM_TO_M;
    if (elevation > 1000000.0) {
      elevation *= ELEVATION_SCALING;
      logDebug("Scaled large elevation from " +
               std::to_string(elevation / ELEVATION_SCALING) + "m to " +
               std::to_string(elevation) + "m");
    }

    return elevation;
  }

  double processElevation(double rawElevation, int row, int col) {
    if (!metrics.isInitialized()) {
      throw std::runtime_error("Terrain metrics not initialized");
    }

    if (row < 0 || row >= static_cast<int>(metrics.relativeHeights.size()) ||
        col < 0 || col >= static_cast<int>(metrics.relativeHeights[0].size())) {
      throw std::runtime_error("Elevation coordinates out of bounds");
    }

    double elevation = validateElevation(rawElevation);

    if (!metrics.baseElevationSet) {
      metrics.baseElevation = elevation;
      metrics.baseElevationSet = true;
      logDebug("Base elevation set to: " +
               std::to_string(metrics.baseElevation) + "m");
      return 0.0;
    }

    double relativeElevation = elevation - metrics.baseElevation;
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

  double calculateSlope(double dx, double dy, double elevChange) {
    double horizontalDist = std::sqrt(dx * dx + dy * dy) * MARS_PIXEL_SIZE;
    if (horizontalDist < ELEVATION_EPSILON)
      return 0.0;

    elevChange = std::min(std::abs(elevChange), MAX_ELEVATION_CHANGE);
    double slope = std::atan2(elevChange, horizontalDist) * 180.0 / M_PI;
    return std::min(slope, MAX_SLOPE);
  }

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
    double lastSignificantElev =
        metrics.relativeHeights[path[0].first][path[0].second];

    for (size_t i = 1; i < path.size() - 1; i++) {
      const auto &curr = path[i];
      double currElev = metrics.relativeHeights[curr.first][curr.second];

      if (std::abs(currElev - lastSignificantElev) > SIGNIFICANT_CHANGE) {
        optimized.push_back(curr);
        lastSignificantElev = currElev;
        continue;
      }

      const auto &prev = path[i - 1];
      const auto &next = path[i + 1];
      double dx1 = curr.first - prev.first;
      double dy1 = curr.second - prev.second;
      double dx2 = next.first - curr.first;
      double dy2 = next.second - curr.second;

      double angle = std::abs(std::atan2(dy2, dx2) - std::atan2(dy1, dx1));
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

  void updatePathMetrics(const std::vector<std::pair<int, int>> &path,
                         const std::vector<std::vector<double>> &heightmap) {
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

      double startElev =
          processElevation(heightmap[path[0].first][path[0].second],
                           path[0].first, path[0].second);
      double endElev =
          processElevation(heightmap[path.back().first][path.back().second],
                           path.back().first, path.back().second);
      metrics.netElevationChange = endElev - startElev;

      for (size_t i = 1; i < path.size(); ++i) {
        const auto &curr = path[i];
        const auto &prev = path[i - 1];

        double dx = curr.first - prev.first;
        double dy = curr.second - prev.second;
        double horizontalDist = std::sqrt(dx * dx + dy * dy) * MARS_PIXEL_SIZE;

        double elev1 = metrics.relativeHeights[prev.first][prev.second];
        double elev2 = metrics.relativeHeights[curr.first][curr.second];
        double elevChange = std::abs(elev2 - elev1);

        double slope = calculateSlope(dx, dy, elevChange);
        double distance = std::sqrt(horizontalDist * horizontalDist +
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

  void writePathData(const std::string &filename,
                     const std::vector<std::pair<int, int>> &path,
                     const std::vector<std::vector<double>> &heightmap,
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
      double elevation = heightmap[coord.first][coord.second] * MM_TO_M;

      // Fix coordinate ordering: swap row/col to match X/Y convention
      int globalY = coord.first + startRow;
      int globalX = coord.second + startCol;

      outFile << globalX << " " << globalY << " " << elevation << " "
              << "\n";
    }

    outFile.close();
    logDebug("Path data written to " + filename);
  }

  void displayDetailedSummary(int startX, int startY, int endX, int endY) {
    const double metersPerPixel = MARS_PIXEL_SIZE;
    const double actualDistance =
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

  void logProgress(const std::string &status, int current, int total) {
    double percent = (current * 100.0) / total;
    progress.updateProgress(status, percent);
  }

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

public:
  explicit Simulator(std::string n) : name(std::move(n)) {
    startTime = std::chrono::steady_clock::now();
  }

  void run(int startX, int startY, int endX, int endY, std::string inFile) {
    displaySimulationHeader(startX, startY, endX, endY);

    /*const std::string demFilePath =
     * "/Users/adamcarlson/Dropbox/Mac/Desktop/Desktop/"*/
    /*                                "nasa-jpl-capstone-2024-2025/prototypes/sim/"*/
    /*                                "Mars_HRSC_MOLA_BlendDEM_Global_200mp_v2.tif";*/
    /*const std::string outputPath =
     * "/Users/adamcarlson/Dropbox/Mac/Downloads";*/

    const std::string demFilePath = inFile;
    const std::string outputPath =
        "/mnt/c/Users/Oscar/Desktop/MEMPA_Personal/output"; // TODO: for now this is hardcoded, ideally it should not
                  // have to exist

    try {
      logDebug("Initializing simulation");
      MEMPA::BUFFDEM demHandler(demFilePath, outputPath);

      // Calculate region with padding
      uint32_t startRow =
          (startY > TERRAIN_PADDING) ? startY - TERRAIN_PADDING : 0;
      uint32_t endRow =
          std::min(static_cast<uint32_t>(endY + TERRAIN_PADDING), 53347u);
      uint32_t startCol =
          (startX > TERRAIN_PADDING) ? startX - TERRAIN_PADDING : 0;
      uint32_t endCol =
          std::min(static_cast<uint32_t>(endX + TERRAIN_PADDING), 106694u);

      logDebug("Loading heightmap for region: (" + std::to_string(startRow) +
               "," + std::to_string(startCol) + ") to (" +
               std::to_string(endRow) + "," + std::to_string(endCol) + ")");

    std::vector<std::pair<double, double>> coordinates = {{startRow, startCol}, {endRow, endCol}};
    double radius_eccentricity = 0.5;
    const std::string outputFilename = "outputRaster";
    bool useSquareClipping = false;


      auto heightmap = demHandler.makeRequest(coordinates, radius_eccentricity, demFilePath, outputPath, outputFilename, useSquareClipping);
          //demHandler.demVector(demFilePath, startRow, endRow, startCol, endCol);

      if (heightmap.empty() || heightmap[0].empty()) {
        throw std::runtime_error("Failed to load heightmap data");
      }

      // Initialize metrics before pathfinding
      metrics.initialize(heightmap.size(), heightmap[0].size());

      logDebug("Heightmap loaded: " + std::to_string(heightmap.size()) + "x" +
               std::to_string(heightmap[0].size()));

      auto localStart =
          convertToLocalCoordinates(startX, startY, startRow, startCol);
      auto localEnd = convertToLocalCoordinates(endX, endY, startRow, startCol);

      if (!isValidCoordinate(localStart, heightmap) ||
          !isValidCoordinate(localEnd, heightmap)) {
        throw std::runtime_error("Start or end point out of bounds");
      }

      Dijkstras dijkstra;
      auto path = dijkstra.dijkstras(heightmap, localStart, localEnd, MAX_SLOPE,
                                     MARS_PIXEL_SIZE);

      if (path.empty()) {
        throw std::runtime_error("No valid path found between points");
      }

      updatePathMetrics(path, heightmap);
      optimizePath(path);
      writePathData("rover_path.txt", path, heightmap, startX, startY, endX,
                    endY, startRow, startCol);

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
    } catch (const std::exception &e) {
      logDebug("Error in run: " + std::string(e.what()));
      throw;
    }
  }
};

int main(int argc, char **argv) {
  // take argus for in an out fixed
  std::string inputFile;
  int startX = 1254;
  int startY = 1265;
  int endX = 1340;
  int endY = 1338;
  int opt;

  while ((opt = getopt(argc, argv, "i:x:y:X:Y:")) != -1) {
    switch (opt) {
    case 'i':
      inputFile = optarg;
      break;
    case 'x':
      startX = std::atoi(optarg);
      break;
    case 'y':
      startY = std::atoi(optarg);
      break;
    case 'X':
      endX = std::atoi(optarg);
      break;
    case 'Y':
      endY = std::atoi(optarg);
      break;
    case '?':
      std::cerr
          << "Usage: " << argv[0]
          << " -i <input_file> -x <startX> -y <startY> -X <endX> -Y <endY>\n";
      return 1;
    }
  }

  try {
    // // Default coordinates for testing
    // int startX = 1254;
    // int startY = 1265;
    // int endX = 1340;
    // int endY = 1338;

    // if (argc >= 5) {
    //   // Override defaults with command line arguments
    //   startX = std::stoi(argv[1]);
    //   startY = std::stoi(argv[2]);
    //   endX = std::stoi(argv[3]);
    //   endY = std::stoi(argv[4]);
    // } else if (argc > 1) {
    //   // If some but not all coordinates provided, show usage
    //   std::cerr << "Usage: " << argv[0] << " <startX> <startY> <endX>
    //   <endY>\n"
    //             << "Valid ranges:\n"
    //             << "  X: 0 to 106,694\n"
    //             << "  Y: 0 to 53,347\n"
    //             << "\nOr run without arguments to use default coordinates:\n"
    //             << "  Start: (" << startX << "," << startY << ")\n"
    //             << "  End:   (" << endX << "," << endY << ")\n";
    //   return 1;
    // }

    // Validate input coordinates
    if (startX < 0 || startY < 0 || endX < 0 || endY < 0 || startX > 106694 ||
        endX > 106694 || startY > 53347 || endY > 53347) {
      throw std::runtime_error("Coordinates out of bounds for Mars DEM");
    }

    std::cout << "NASA JPL Rover Proxy" << std::endl;
    std::cout << "Input coordinates: (" << startX << "," << startY << ") to ("
              << endX << "," << endY << ")" << std::endl;

    Simulator sim("NASA JPL Rover Simulator");
    sim.run(startX, startY, endX, endY, inputFile);
    return 0;

  } catch (const std::exception &e) {
    std::cerr << "\n Fatal error: " << e.what() << std::endl;
    return 1;
  }
}

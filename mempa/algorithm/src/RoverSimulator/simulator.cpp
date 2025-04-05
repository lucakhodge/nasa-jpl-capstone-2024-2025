#include "../DemHandler/DemHandler.hpp"
#include "../search_algorithms/SearchContext.h"
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
#include <tuple>
#include <typeinfo>
#include <unistd.h>
#include <vector>

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
   * @brief function to change the displayed progress spinner
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
 * @brief The Simulator class, this class is used to abstract the starting and
 * goal conditions from the DEM files and search algorithm used to traverse the
 * DEM file
 *
 * @author Adam Carlson and Oscar Mikus
 */
class Simulator {

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
  std::string name;
  std::chrono::steady_clock::time_point startTime;
  ProgressTracker progress;
  std::pair<int, int> currentPos;

  /**
   * @brief Given a coordinate in std::pair<int, int> and a heightmap
   * std::vector<std::vector<float>> retuns a truth value based on if the
   * coordinates pair is within the heightmap.
   *
   * @author Adam Carlson
   */
  bool isValidCoordinate(const std::pair<int, int> &coord,
                         const std::vector<std::vector<float>> &heightmap) {

    if (heightmap.empty() || heightmap[0].empty()) {
      throw std::runtime_error("Invalid heightmap");
    }

    bool valid = coord.first >= 0 &&
                 coord.first < static_cast<int>(heightmap.size()) &&
                 coord.second >= 0 &&
                 coord.second < static_cast<int>(heightmap[0].size());

    if (!valid) {
      logDebug("Invalid coordinate (" + std::to_string(coord.first) + "," +
               std::to_string(coord.second) +
               ") heightmap.size(): " + std::to_string(heightmap.size()) +
               " heightmap[0].size(): " + std::to_string(heightmap[0].size()));
    }
    return valid;
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
   * @brief Converts pixel coordinates from an entire DEM file to the local
   * pixel coordinates of a chunk of a DEM file
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
   * @brief Used to change local pixel coordinates of a small DEM chunk back to
   * the pixel coordinates of an entire DEM file
   *
   * @author Oscar Mikus
   */
  std::pair<int, int> convertToGlobalCoordinates(int localX, int localY,
                                                 uint32_t startRow,
                                                 uint32_t startCol) {
    int globalX = localX + startCol;
    int globalY = localY + startRow;
    logDebug("Converting local (" + std::to_string(localX) + "," +
             std::to_string(localY) + ") to global (" +
             std::to_string(globalX) + "," + std::to_string(globalY) + ")");
    return {globalX, globalY};
  }

  /**
   * @brief updates the ProgressTracker inside of the Simulator class with new
   * information
   *
   * @author Adam Carlson
   */
  void logProgress(const std::string &status, int current, int total) {
    float percent = (current * 100.0) / total;
    progress.updateProgress(status, percent);
  }

  /**
   * @brief calculates the coordinates needed to create a region by the DEM
   * handler function
   *
   * @author Oscar Mikus, Adam Carlson
   */
  std::tuple<int, int, int, int>
  calculateRegionCordinates(int startX, int startY, int endX, int endY) {
    int startRow = (startY > TERRAIN_PADDING) ? startY - TERRAIN_PADDING : 0;
    int endRow = std::min(static_cast<int>(endY + TERRAIN_PADDING),
                          53347); // TODO make 53347u a parameter of some sort
    int startCol = (startX > TERRAIN_PADDING) ? startX - TERRAIN_PADDING : 0;
    int endCol = std::min(static_cast<int>(endX + TERRAIN_PADDING),
                          106694); // TODO make 106694u a parameter of some sort

    std::cout << startRow << " " << startCol << " " << endRow << " " << endCol
              << std::endl;
    return std::make_tuple(startRow, startCol, endRow, endCol);
  }

  /**
   * @brief this function caluculates the closest cordinate inside of a square
   * area to an end point
   *
   * @author Oscar Mikus
   */
  std::pair<int, int> calculateDemNavToHereRadius(int startX, int startY,
                                                  int endX, int endY,
                                                  int radius) {
    int left = startX - radius;
    int right = startX + radius;
    int bottom = startY - radius;
    int top = startY + radius;

    int closestX = std::clamp(endX, left, right);
    int closestY = std::clamp(endY, bottom, top);

    return std::make_pair(closestX, closestY);
  }

  /**
   * @brief this function calculates the coordinates for the closest postion to
   * the actual goal coordinate within the limitations of the max dem size based
   * on a max number of pixels to represent memory limitations
   *
   * @author Oscar Mikus
   */
  std::pair<int, int> calculateDemNavToHere(int startX, int startY, int endX,
                                            int endY,
                                            int maxDemRegionPixelCount) {
    if (maxDemRegionPixelCount < (TERRAIN_PADDING * TERRAIN_PADDING)) {
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
      // Try adjusting the width or height while keeping the area under the
      // limit
      float aspectRatio = static_cast<float>(width) / height;

      // Try adjusting the width first
      int newWidth = std::min(
          width, static_cast<int>(sqrt(maxDemRegionPixelCount * aspectRatio)));
      int newHeight = maxDemRegionPixelCount / newWidth;

      if (newWidth * newHeight > maxDemRegionPixelCount) {
        // If the new width and height do not satisfy the constraint, adjust
        // height
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
   * @brief Default Constructor for Simulator class. Initialize the start time
   * for the simulator
   *
   * @author Adam Carlson
   */
  explicit Simulator(std::string n) : name(std::move(n)) {
    startTime = std::chrono::steady_clock::now();
  }

  /**
   * @brief Main function to call within the Simulator class.  This will
   * navigate from the startpoint to the endpoint in a given DEM file whie
   * adhearing to the max pixel size constraint
   *
   * @author Adam Carlson, Oscar Mikus
   */
  std::vector<std::pair<int, int>>
  runWithSquareRadius(int startX, int startY, int endX, int endY,
                      mempa::DemHandler &demHandler, int radius,
                      SearchAlgorithm *searchStratToUse) {
    displaySimulationHeader(startX, startY, endX, endY);

    /*const std::string demFilePath =
     * "/Users/adamcarlson/Dropbox/Mac/Desktop/Desktop/"*/
    /*                                "nasa-jpl-capstone-2024-2025/prototypes/sim/"*/
    /*                                "Mars_HRSC_MOLA_BlendDEM_Global_200mp_v2.tif";*/
    /*const std::string outputPath =
     * "/Users/adamcarlson/Dropbox/Mac/Downloads";*/

    try {
      logDebug("Initializing simulation");
      // MEMPA::BUFFDEM demHandler(demFilePath, outputPath);
      currentPos.first = startX;
      currentPos.second = startY;

      std::tuple<int, int, int, int> regionCordinates;
      std::pair<int, int> DemNavToHere;
      std::vector<std::pair<int, int>> path;
      std::vector<std::vector<float>> heightmap;
      SearchContext search(searchStratToUse);

      search.setStrategy(searchStratToUse);

      // start looping here
      while (currentPos.first != endX && currentPos.second != endY) {

        // Calculate region with padding
        DemNavToHere = calculateDemNavToHereRadius(
            currentPos.first, currentPos.second, endX, endY, radius);
        regionCordinates =
            calculateRegionCordinates(currentPos.first, currentPos.second,
                                      DemNavToHere.first, DemNavToHere.second);

        // logDebug("Loading heightmap for square area at (" + currentPos.first
        // + ", " + currentPos.second + ") with radius " + radius);

        heightmap = demHandler.readSquareChunk(currentPos, radius);

        if (heightmap.empty() || heightmap[0].empty()) {
          throw std::runtime_error("Failed to load heightmap data");
        }

        logDebug("Heightmap loaded: " + std::to_string(heightmap.size()) + "x" +
                 std::to_string(heightmap[0].size()));

        auto localStart = convertToLocalCoordinates(
            currentPos.first, currentPos.second, std::get<0>(regionCordinates),
            std::get<1>(regionCordinates));
        auto localEnd = convertToLocalCoordinates(
            DemNavToHere.first, DemNavToHere.second,
            std::get<0>(regionCordinates), std::get<1>(regionCordinates));

        if (!isValidCoordinate(localStart, heightmap) ||
            !isValidCoordinate(localEnd, heightmap)) {
          throw std::runtime_error("Start or end point out of bounds");
        }

        logDebug("Starting to use strategy");
        search.executeStrategyReset();
        logDebug("Did first strategy thing");
        search.executeStrategySetUpAlgo(heightmap, localStart, localEnd,
                                        MAX_SLOPE, MARS_PIXEL_SIZE);
        logDebug("Did second strategy thing");

        // auto path = dijkstra.dijkstras(heightmap, localStart, localEnd,
        // MAX_SLOPE,
        //                                MARS_PIXEL_SIZE);

        std::pair<int, int> localStep;
        std::pair<int, int> globalStep;
        while (true) {
          localStep = search.executeStrategyGetStep();
          if (localStep == std::make_pair(-1, -1)) {
            break;
          }
          globalStep = convertToGlobalCoordinates(
              localStep.first, localStep.second, std::get<0>(regionCordinates),
              std::get<1>(regionCordinates));
          path.push_back(globalStep);
        }

        // need to set new start coordinates here
        currentPos = globalStep;
      }

      if (path.empty()) {
        throw std::runtime_error("No valid path found between points");
      }

      regionCordinates = calculateRegionCordinates(startX, startY, endX, endY);
      heightmap = demHandler.readRectangleChunk(
          std::make_pair(std::make_pair(std::get<0>(regionCordinates),
                                        std::get<1>(regionCordinates)),
                         std::make_pair(std::get<2>(regionCordinates),
                                        std::get<3>(regionCordinates))),
          0);

      return path;
    } catch (const std::exception &e) {
      logDebug("Error in run: " + std::string(e.what()));
      throw;
    }
  }

  /**
   * @brief Main function to call within the Simulator class.  This will
   * navigate from the startpoint to the endpoint in a given DEM file while
   * adhering to the max pixel size constraint
   *
   * @author Adam Carlson, Oscar Mikus
   */
  std::vector<std::pair<int, int>> run(int startX, int startY, int endX,
                                       int endY, mempa::DemHandler &demHandler,
                                       int maxDemRegionPixelCount,
                                       SearchAlgorithm *searchStratToUse) {
    displaySimulationHeader(startX, startY, endX, endY);

    /*const std::string demFilePath =
     * "/Users/adamcarlson/Dropbox/Mac/Desktop/Desktop/"*/
    /*                                "nasa-jpl-capstone-2024-2025/prototypes/sim/"*/
    /*                                "Mars_HRSC_MOLA_BlendDEM_Global_200mp_v2.tif";*/
    /*const std::string outputPath =
     * "/Users/adamcarlson/Dropbox/Mac/Downloads";*/

    try {
      logDebug("Initializing simulation");
      // MEMPA::BUFFDEM demHandler(demFilePath, outputPath);
      currentPos.first = startX;
      currentPos.second = startY;

      std::tuple<int, int, int, int> regionCordinates;
      std::pair<int, int> DemNavToHere;
      std::vector<std::pair<int, int>> path;
      std::vector<std::vector<float>> heightmap;
      SearchContext search(searchStratToUse);

      search.setStrategy(searchStratToUse);

      // start looping here
      while (currentPos.first != endX && currentPos.second != endY) {

        // Calculate region with padding
        DemNavToHere =
            calculateDemNavToHere(currentPos.first, currentPos.second, endX,
                                  endY, maxDemRegionPixelCount);
        regionCordinates =
            calculateRegionCordinates(currentPos.first, currentPos.second,
                                      DemNavToHere.first, DemNavToHere.second);

        logDebug("Loading heightmap for region: (" +
                 std::to_string(std::get<0>(regionCordinates)) + "," +
                 std::to_string(std::get<1>(regionCordinates)) + ") to (" +
                 std::to_string(std::get<2>(regionCordinates)) + "," +
                 std::to_string(std::get<3>(regionCordinates)) + ")");

        heightmap = demHandler.readRectangleChunk(
            std::make_pair(std::make_pair(std::get<0>(regionCordinates),
                                          std::get<1>(regionCordinates)),
                           std::make_pair(std::get<2>(regionCordinates),
                                          std::get<3>(regionCordinates))),
            0);

        if (heightmap.empty() || heightmap[0].empty()) {
          throw std::runtime_error("Failed to load heightmap data");
        }

        logDebug("Heightmap loaded: " + std::to_string(heightmap.size()) + "x" +
                 std::to_string(heightmap[0].size()));

        auto localStart = convertToLocalCoordinates(
            currentPos.first, currentPos.second, std::get<0>(regionCordinates),
            std::get<1>(regionCordinates));
        auto localEnd = convertToLocalCoordinates(
            DemNavToHere.first, DemNavToHere.second,
            std::get<0>(regionCordinates), std::get<1>(regionCordinates));

        if (!isValidCoordinate(localStart, heightmap) ||
            !isValidCoordinate(localEnd, heightmap)) {
          throw std::runtime_error("Start or end point out of bounds");
        }

        logDebug("Starting to use strategy");
        search.executeStrategyReset();
        logDebug("Did first strategy thing");
        search.executeStrategySetUpAlgo(heightmap, localStart, localEnd,
                                        MAX_SLOPE, MARS_PIXEL_SIZE);
        logDebug("Did second strategy thing");

        // auto path = dijkstra.dijkstras(heightmap, localStart, localEnd,
        // MAX_SLOPE,
        //                                MARS_PIXEL_SIZE);

        std::pair<int, int> localStep;
        std::pair<int, int> globalStep;
        while (true) {
          localStep = search.executeStrategyGetStep();
          if (localStep == std::make_pair(-1, -1)) {
            break;
          }
          globalStep = convertToGlobalCoordinates(
              localStep.first, localStep.second, std::get<0>(regionCordinates),
              std::get<1>(regionCordinates));
          path.push_back(globalStep);
        }

        // need to set new start coordinates here
        currentPos = globalStep;
      }

      if (path.empty()) {
        throw std::runtime_error("No valid path found between points");
      }

      regionCordinates = calculateRegionCordinates(startX, startY, endX, endY);
      heightmap = demHandler.readRectangleChunk(
          std::make_pair(std::make_pair(std::get<0>(regionCordinates),
                                        std::get<1>(regionCordinates)),
                         std::make_pair(std::get<2>(regionCordinates),
                                        std::get<3>(regionCordinates))),
          0);

      // update path to local coordinates in this final heightmap
      for (std::pair<int, int> &step : path) {
        step = convertToLocalCoordinates(step.first, step.second,
                                         std::get<0>(regionCordinates),
                                         std::get<1>(regionCordinates));
      }

      // optimizePath(path);

      return path;
    } catch (const std::exception &e) {
      logDebug("Error in run: " + std::string(e.what()));
      throw;
    }
  }
};

// current way to run and test the Simulator.
//  int main(int argc, char **argv) {
//    // take argus for in an out fixed
//    std::string inputFile;
//    int startX = 1254;
//    int startY = 1265;
//    int endX = 1340;
//    int endY = 1338;
//    int opt;

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
//           << " -i <input_file> -x <startX> -y <startY> -X <endX> -Y
//           <endY>\n";
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
//     //             << "\nOr run without arguments to use default
//     coordinates:\n"
//     //             << "  Start: (" << startX << "," << startY << ")\n"
//     //             << "  End:   (" << endX << "," << endY << ")\n";
//     //   return 1;
//     // }

//     // Validate input coordinates
//     if (startX < 0 || startY < 0 || endX < 0 || endY < 0 || startX > 106694
//     ||
//         endX > 106694 || startY > 53347 || endY > 53347) {
//       throw std::runtime_error("Coordinates out of bounds for Mars DEM");
//     }

//     std::cout << "NASA JPL Rover Proxy" << std::endl;
//     std::cout << "Input coordinates: (" << startX << "," << startY << ") to
//     ("
//               << endX << "," << endY << ")" << std::endl;

//     Simulator sim("NASA JPL Rover Simulator");

//     mempa::DemHandler demHandler = mempa::DemHandler(inputFile.c_str());
//     Dijkstras searchAlgoritm;

//     sim.runWithSquareRadius(startX, startY, endX, endY, demHandler, 20,
//     &searchAlgoritm); return 0;

//   } catch (const std::exception &e) {
//     std::cerr << "\n Fatal error: " << e.what() << std::endl;
//     return 1;
//   }
// }

#include "JsonPathLogger.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

void JsonPathLogger::logPath(const std::string &filename,
                             const std::vector<std::pair<int, int>> &path,
                             std::pair<int, int> startPoint,
                             std::pair<int, int> endPoint,
                             const TerrainMetrics &metrics) {

  // Create separate filenames for path data and analytics
  // Remove .txt extension if present
  std::string baseName = filename;
  size_t pos = baseName.find(".txt");
  if (pos != std::string::npos) {
    baseName = baseName.substr(0, pos);
  }

  std::string pathFilename = baseName + "_path.json";
  std::string analyticsFilename = baseName + "_analytics.json";

  // Create path JSON
  nlohmann::json pathJson;
  pathJson["metadata"] = {{"timestamp", std::time(nullptr)},
                          {"start", {startPoint.first, startPoint.second}},
                          {"end", {endPoint.first, endPoint.second}}};

  pathJson["path"] = nlohmann::json::array();
  for (const auto pathStep : path) {

    pathJson["path"].push_back({{"x", pathStep.first}, {"y", pathStep.second}});
  }

  // Create analytics JSON
  nlohmann::json analyticsJson;
  analyticsJson["metrics"] = {
      {"totalDistance", metrics.totalDistance},
      {"horizontalDistance", metrics.horizontalDistance},
      {"totalElevationChange", metrics.totalElevationChange},
      {"netElevationChange", metrics.netElevationChange},
      {"maxSlope", metrics.maxSlope},
      {"averageSlope", metrics.averageSlope},
      {"energyCost", metrics.energyCost},
      {"baseElevation", metrics.baseElevation},
      {"waypointCount", metrics.waypointCount}};

  // Write path JSON to file
  std::ofstream pathFile(pathFilename);
  if (!pathFile) {
    throw std::runtime_error("Failed to create path output file: " +
                             pathFilename);
  }
  pathFile << pathJson.dump(2); // Pretty print with 2-space indent
  pathFile.close();

  // Write analytics JSON to file
  std::ofstream analyticsFile(analyticsFilename);
  if (!analyticsFile) {
    throw std::runtime_error("Failed to create analytics output file: " +
                             analyticsFilename);
  }
  analyticsFile << analyticsJson.dump(2);
  analyticsFile.close();

  // Also create a combined file for backward compatibility
  std::ofstream combinedFile(filename);
  if (!combinedFile) {
    throw std::runtime_error("Failed to create combined output file: " +
                             filename);
  }

  nlohmann::json combinedJson = pathJson;
  combinedJson["metrics"] = analyticsJson["metrics"];
  combinedFile << combinedJson.dump(2);
  combinedFile.close();
}

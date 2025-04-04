#include "path_logger.h"
#include "terrain_metrics.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::unique_ptr<PathLogger> PathLogger::createLogger(bool useJson) {
    if (useJson) {
        return std::make_unique<JsonPathLogger>();
    } else {
        return std::make_unique<TextPathLogger>();
    }
}

void TextPathLogger::logPath(
    const std::string& filename,
    const std::vector<std::pair<int, int>>& path,
    const std::vector<std::vector<float>>& heightmap,
    int startX, int startY, int endX, int endY,
    uint32_t startRow, uint32_t startCol,
    const TerrainMetrics& metrics) {
    
    // Original text output implementation
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
    outFile << "#   Horizontal Distance: " << metrics.horizontalDistance << " m\n";
    outFile << "#   Total Elevation Change: " << metrics.totalElevationChange << " m\n";
    outFile << "#   Net Elevation Change: " << metrics.netElevationChange << " m\n";
    outFile << "#   Maximum Slope: " << metrics.maxSlope << "°\n";
    outFile << "#   Average Slope: " << metrics.averageSlope << "°\n";
    outFile << "#   Energy Cost: " << metrics.energyCost << "\n";
    outFile << "#   Base Elevation: " << metrics.baseElevation << " m\n";
    outFile << "#   Waypoints: " << metrics.waypointCount << "\n\n";
    outFile << "# X Y Elevation(m) \n";

    for (size_t i = 0; i < path.size(); ++i) {
        const auto& coord = path[i];
        float elevation = heightmap[coord.first][coord.second] * 0.001; // MM_TO_M

        // Convert to global coordinates
        int globalY = coord.first + startRow;
        int globalX = coord.second + startCol;

        outFile << globalX << " " << globalY << " " << elevation << "\n";
    }

    outFile.close();
}

void JsonPathLogger::logPath(
    const std::string& filename,
    const std::vector<std::pair<int, int>>& path,
    const std::vector<std::vector<float>>& heightmap,
    int startX, int startY, int endX, int endY,
    uint32_t startRow, uint32_t startCol,
    const TerrainMetrics& metrics) {
    
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
    json pathJson;
    pathJson["metadata"] = {
        {"timestamp", std::time(nullptr)},
        {"start", {startX, startY}},
        {"end", {endX, endY}}
    };
    
    pathJson["path"] = json::array();
    for (const auto& [row, col] : path) {
        float elevation = heightmap[row][col] * 0.001; // MM_TO_M
        
        // Convert to global coordinates
        int globalY = row + startRow;
        int globalX = col + startCol;
        
        pathJson["path"].push_back({
            {"x", globalX},
            {"y", globalY},
            {"elevation", elevation}
        });
    }
    
    // Create analytics JSON
    json analyticsJson;
    analyticsJson["metrics"] = {
        {"totalDistance", metrics.totalDistance},
        {"horizontalDistance", metrics.horizontalDistance},
        {"totalElevationChange", metrics.totalElevationChange},
        {"netElevationChange", metrics.netElevationChange},
        {"maxSlope", metrics.maxSlope},
        {"averageSlope", metrics.averageSlope},
        {"energyCost", metrics.energyCost},
        {"baseElevation", metrics.baseElevation},
        {"waypointCount", metrics.waypointCount}
    };
    
    // Write path JSON to file
    std::ofstream pathFile(pathFilename);
    if (!pathFile) {
        throw std::runtime_error("Failed to create path output file: " + pathFilename);
    }
    pathFile << pathJson.dump(2); // Pretty print with 2-space indent
    pathFile.close();
    
    // Write analytics JSON to file
    std::ofstream analyticsFile(analyticsFilename);
    if (!analyticsFile) {
        throw std::runtime_error("Failed to create analytics output file: " + analyticsFilename);
    }
    analyticsFile << analyticsJson.dump(2);
    analyticsFile.close();
    
    // Also create a combined file for backward compatibility
    std::ofstream combinedFile(filename);
    if (!combinedFile) {
        throw std::runtime_error("Failed to create combined output file: " + filename);
    }
    
    json combinedJson = pathJson;
    combinedJson["metrics"] = analyticsJson["metrics"];
    combinedFile << combinedJson.dump(2);
    combinedFile.close();
}
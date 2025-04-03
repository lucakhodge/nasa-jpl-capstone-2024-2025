#pragma once

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <fstream>

// Forward declaration
class TerrainMetrics;

/**
 * @brief Abstract base class for path logging functionality
 * 
 * @author Adam Carlson
 */
class PathLogger {
public:
    virtual ~PathLogger() = default;
    
    /**
     * @brief Logs path data to a file
     * 
     * @param filename Base filename for output files
     * @param path Vector of path coordinates
     * @param heightmap 2D vector containing terrain elevation data
     * @param startX Starting X coordinate (global)
     * @param startY Starting Y coordinate (global)
     * @param endX Ending X coordinate (global)
     * @param endY Ending Y coordinate (global)
     * @param startRow Row offset for converting local to global coordinates
     * @param startCol Column offset for converting local to global coordinates
     * @param metrics Terrain metrics calculated for the path
     */
    virtual void logPath(
        const std::string& filename,
        const std::vector<std::pair<int, int>>& path,
        const std::vector<std::vector<float>>& heightmap,
        int startX, int startY, int endX, int endY,
        uint32_t startRow, uint32_t startCol,
        const TerrainMetrics& metrics) = 0;
        
    static std::unique_ptr<PathLogger> createLogger(bool useJson);
};

/**
 * @brief Outputs path data in text format (original format)
 */
class TextPathLogger : public PathLogger {
public:
    void logPath(
        const std::string& filename,
        const std::vector<std::pair<int, int>>& path,
        const std::vector<std::vector<float>>& heightmap,
        int startX, int startY, int endX, int endY,
        uint32_t startRow, uint32_t startCol,
        const TerrainMetrics& metrics) override;
};

/**
 * @brief Outputs path data in JSON format with separate files for analytics and path
 */
class JsonPathLogger : public PathLogger {
public:
    void logPath(
        const std::string& filename,
        const std::vector<std::pair<int, int>>& path,
        const std::vector<std::vector<float>>& heightmap,
        int startX, int startY, int endX, int endY,
        uint32_t startRow, uint32_t startCol,
        const TerrainMetrics& metrics) override;
};
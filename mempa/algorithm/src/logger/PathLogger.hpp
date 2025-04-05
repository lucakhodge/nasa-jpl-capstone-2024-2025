#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

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
  virtual void logPath(const std::string &filename,
                       const std::vector<std::pair<int, int>> &path) = 0;

  static std::unique_ptr<PathLogger> createLogger(bool useJson);
};

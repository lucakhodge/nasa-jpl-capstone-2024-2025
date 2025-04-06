/* Local Header */
#include "RoverSimulator.hpp"

/* C++ Standard Libraries */
#include <cmath>
#include <vector>
#include <utility>

namespace mempa
{
    /**
     * @brief Ensures that a requested coordinate is within the 2D vector of read values.
     *
     * @param vecCoordinate Coordinate of vector indices requested. Should be in (x, y) format.
     * @param rasterVector The vector to check if vecCoordinate is valid within.
     * @return true Coordinate is within the vector.
     * @return false Coordinate is outside the vector, would segfault.
     */
    inline bool RoverSimulator::validateCoordinate(const std::pair<int, int> vecCoordinate, const std::vector<std::vector<float>> rasterVector) const noexcept
    {
        return ((vecCoordinate.first >= 0) && (static_cast<size_t>(vecCoordinate.first) < rasterVector.front().size())) &&
               ((vecCoordinate.second >= 0) && (static_cast<size_t>(vecCoordinate.second) < rasterVector.size()));
    }

    /**
     * @brief Checks if the elevation value is valid.
     *
     * @param elevationValue The elevation value to be checked.
     *
     * @return true The elevation is valid.
     * @return false The elevation is not a number.
     */
    inline bool RoverSimulator::validateElevation(const float elevationValue) const noexcept
    {
        return !std::isnan(elevationValue);
    }

    /**
     * @brief Get the difference between two points.
     *
     * @param coordinate1 Image based coordinate one.
     * @param coordinate2 Image based coordinate two.
     * @return std::pair<int, int> X difference, Y difference.
     */
    inline std::pair<int, int> RoverSimulator::coordinateDifference(const std::pair<int, int> coordinate1, const std::pair<int, int> coordinate2) const noexcept
    {
        return std::pair<int, int>(coordinate2.first - coordinate1.first, coordinate2.second - coordinate1.second);
    }

    /**
     * @brief Get the top left coordiante of a vector in global coordinate space.
     *
     * @param globalCoordinate Current coordinate (must be center of 2D vector chunk).
     * @param buffer Buffer used for vector chunk.
     * @return std::pair<int, int> Top left corner of vector chunk in global coordinates.
     */
    inline std::pair<int, int> RoverSimulator::globalVectorCorner(const std::pair<int, int> globalCoordinate, const int buffer) const noexcept
    {
        return std::pair<int, int>(globalCoordinate.first - buffer, globalCoordinate.second - buffer);
    }
}

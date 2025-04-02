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
     * @return @c true Coordinate is within the vector.
     * @return @c false Coordinate is outside the vector, would segfault.
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
     * @return @c true The elevation is valid.
     * @return @c false The elevation is not a number.
     */
    inline bool RoverSimulator::validateElevation(const float elevationValue) const noexcept
    {
        return !std::isnan(elevationValue);
    }
}

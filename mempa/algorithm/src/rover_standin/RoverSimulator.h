#pragma once
#include "DemHandler.h"
#include "SearchAlgorithm.h"
#include <cmath>
#include <utility>

namespace mempa
{
    /**
     * @brief Attempt at a revision of the RoverSimulator file.
     *
     * @author Ryan Wagster <rywa2447@colorado.edu>
     */
    class RoverSimulator
    {
    private:
        const DemHandler *elevationRaster;       /* Handler for the DEM file containing elevation data. */
        const DemHandler *slopeRaster;           /* Handler for the Slope raster derived from the DEM data. */
        const std::pair<int, int> startPosition; /* The rover's initial image-based coordinate position. */
        const std::pair<int, int> endPosition;   /* The rover's image-based coordinate desination. */
        std::pair<int, int> currentPosition;     /* The rover's current image-based coordinate position. */

        /**
         * @brief Checks if the elevation value is valid.
         *
         * @param elevationValue The elevation value to be checked.
         *
         * @return @c true The elevation is valid.
         * @return @c false The elevation is not a number.
         */
        inline bool validateElevation(const float elevationValue) const noexcept
        {
            return !std::isnan(elevationValue);
        }

        /**
         * @brief Ensures that a requested coordinate is within the 2D vector of read values.
         *
         * @param vecCoordinate Coordinate of vector indices requested. Should be in (x, y) format.
         * @param rasterVector The vector to check if vecCoordinate is valid within.
         * @return @c true Coordinate is within the vector.
         * @return @c false Coordinate is outside the vector, would segfault.
         */
        inline bool validateCoordinate(const std::pair<int, int> vecCoordinate, const std::vector<std::vector<float>> rasterVector) const noexcept
        {
            return ((vecCoordinate.first >= 0) && (vecCoordinate.first < static_cast<int>(rasterVector.front().size()))) &&
                   ((vecCoordinate.second >= 0) && (vecCoordinate.second < static_cast<int>(rasterVector.size())));
        }

    protected:
        /* RoverSimulator is not designed to be subclassed. */

    public:
        explicit RoverSimulator(const DemHandler *elevationRaster, const DemHandler *slopeRaster, std::pair<double, double> startPosition, std::pair<double, double> endPosition) noexcept;
        void runSimulator(SearchAlgorithm *pathfinder);
    };
}

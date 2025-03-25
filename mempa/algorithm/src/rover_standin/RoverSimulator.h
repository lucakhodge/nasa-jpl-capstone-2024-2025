#pragma once
#include "DemHandler.h"
#include <utility>

namespace mempa
{
    class RoverSimulator
    {
    private:
        const std::pair<double, double> geoStartPos; /* The rover's initial geo-based coordinate position. */
        const std::pair<double, double> geoEndPos;   /* The rover's geo-based coordinate desination. */
        const std::pair<int, int> startPosition;     /* The rover's initial image-based coordinate position. */
        const std::pair<int, int> endPosition;       /* The rover's image-based coordinate desination. */
        std::pair<int, int> currentPosition;         /* The rover's current image-based coordinate position. */
        const DemHandler *elevationRaster;           /* Handler for the DEM file containing elevation data. */
        const DemHandler *slopeRaster;               /* Handler for the Slope raster derived from the DEM data. */
        const double imageResolution;                /* Image resolution in meters. */
        const double minElevation;                   /* Minimum elevation value in meters. */
        const double maxElevation;                   /* Maximum elevation value in meters. */

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

    protected:
        /* RoverSimulator is not designed to be subclassed. */

    public:
        explicit RoverSimulator(std::pair<std::pair<double, double>, std::pair<double, double>> geoCoords,
                                const DemHandler *elevationRaster, const DemHandler *slopeRaster) noexcept;
    };
}

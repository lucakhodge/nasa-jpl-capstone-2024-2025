#pragma once

/* mempa::DemHandler */
#include "../DemHandler/DemHandler.hpp"

/* mempa::SearchAlgorithm */
#include "../search_algorithms/SearchAlgorithm.h"

/* C++ Standard Libraries */
#include <cmath>
#include <vector>
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
        const DemHandler *elevationRaster;                              /* Handler for the DEM file containing elevation data. */
        const DemHandler *slopeRaster;                                  /* Handler for the Slope raster derived from the DEM data. */
        const double imageResolution;                                   /* Raster image resolution in meters. */
        const std::pair<int, int> startPosition;                        /* The rover's initial image-based coordinate position. */
        const std::pair<int, int> endPosition;                          /* The rover's image-based coordinate desination. */
        std::pair<int, int> currentPosition;                            /* The rover's current image-based coordinate position. */
        static inline constexpr float MAX_SLOPE = 35.0f;                /* Maximum tolerable slope for the rover.*/
        static inline constexpr std::pair<int, int> BREAK_STEP{-1, -1}; /* The value that a pathfinding algorithm returns when it is complete. */
        inline bool validateCoordinate(std::pair<int, int> vecCoordinate, std::vector<std::vector<float>> rasterVector) const noexcept;

    protected:
        /* RoverSimulator is not designed to be subclassed. */

    public:
        explicit RoverSimulator(const DemHandler *elevationRaster, const DemHandler *slopeRaster, std::pair<double, double> startPosition, std::pair<double, double> endPosition) noexcept;
        std::vector<std::pair<int, int>> runSimulator(SearchAlgorithm *algorithmType, int buffer);
        inline bool validateElevation(float elevationValue) const noexcept;
        inline std::pair<int, int> coordinateDifference(std::pair<int, int> coordinate1, std::pair<int, int> coordinate2) const noexcept;
    };
}

#include "RoverSimulator.inl"

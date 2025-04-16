#pragma once
#define PREPROCESS_SLOPE false /* Enable (true) or disable (false) the use of a preprocessed slope raster. */

/* mempa::DemHandler */
#include "../dem-handler/DemHandler.hpp"

/* mempa::SearchAlgorithm */
#include "../rover-pathfinding-module/SearchAlgorithm.hpp"

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
        const DemHandler *elevationRaster; /* Handler for the DEM file containing elevation data. */
#if PREPROCESS_SLOPE
        const DemHandler *slopeRaster; /* Handler for the Slope raster derived from the DEM data. */
#endif
        const double imageResolution;                                   /* Raster image resolution in meters. */
        const std::pair<int, int> startPosition;                        /* The rover's initial image-based coordinate position. */
        const std::pair<int, int> goalPosition;                         /* The rover's image-based coordinate desination. */
        std::pair<int, int> currentPosition;                            /* The rover's current image-based coordinate position. */
        inline static constexpr std::pair<int, int> BREAK_STEP{-1, -1}; /* The value that a pathfinding algorithm returns when it is complete. */

    protected:
        /* RoverSimulator is not designed to be subclassed. */

    public:
#if PREPROCESS_SLOPE
        explicit RoverSimulator(const DemHandler *elevationRaster, const DemHandler *slopeRaster, std::pair<double, double> startPosition, std::pair<double, double> goalPosition) noexcept;
#else
        explicit RoverSimulator(const DemHandler *elevationRaster, std::pair<double, double> startPosition, std::pair<double, double> goalPosition) noexcept;
#endif
        std::vector<std::pair<int, int>> runSimulator(SearchAlgorithm *algorithmType, float max_slope, int buffer);
        inline bool validateElevation(float elevationValue) const noexcept;
        inline bool validateCoordinate(std::pair<int, int> vecCoordinate, std::vector<std::vector<float>> rasterVector) const noexcept;
        inline std::pair<int, int> coordinateDifference(std::pair<int, int> coordinate1, std::pair<int, int> coordinate2) const noexcept;
        inline std::pair<int, int> globalVectorCorner(std::pair<int, int> globalCoordinate, int buffer) const noexcept;
    };
}
#include "RoverSimulator.inl"

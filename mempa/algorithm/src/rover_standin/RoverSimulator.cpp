#include "RoverSimulator.h"
#include "DemHandler.h"
#include <utility>

namespace mempa
{
    /**
     * @brief Construct a new Rover Simulator:: Rover Simulator object
     *
     * @param geoCoords A pair of double pairs, representing coordinates. The first coordinate pair shall be the starting position, the second coordinate pair shall be the destination position.
     * @param elevationRaster Pointer to the @c DemHandler object representing the DEM raster.
     * @param slopeRaster Pointer to the @c DemHandler object representing the Slope Data raster.
     */
    RoverSimulator::RoverSimulator(const std::pair<std::pair<double, double>, std::pair<double, double>> geoCoords,
                                   const DemHandler *elevationRaster, const DemHandler *slopeRaster) noexcept
        : geoStartPos(geoCoords.first), geoEndPos(geoCoords.second),
          elevationRaster(elevationRaster), slopeRaster(slopeRaster),
          startPosition(elevationRaster->transformCoordinates(geoStartPos)),
          endPosition(elevationRaster->transformCoordinates(geoEndPos)),
          imageResolution(elevationRaster->getImageResolution()),
          minElevation(elevationRaster->getMinElevation()),
          maxElevation(elevationRaster->getMaxElevation())
    {
        currentPosition = startPosition;
    }
}

/* Local Header */
#include "DemHandler.hpp"

/* libgdal-dev */
#include <gdal_priv.h>

namespace mempa
{
    /**
     * @brief Get the total X size of the raster.
     *
     * @return int Number of pixels per raster row.
     * 
     * @author Ryan Wagster <rywa2447@colorado.edu>
     */
    inline int DemHandler::getXSize() const noexcept
    {
        return poBand->GetXSize();
    }

    /**
     * @brief Get the total Y size of the raster.
     *
     * @return int Number of pixels per raster column.
     * 
     * @author Ryan Wagster <rywa2447@colorado.edu>
     */
    inline int DemHandler::getYSize() const noexcept
    {
        return poBand->GetYSize();
    }

#if DEMHANDLER_MINMAX
    /**
     * @brief Get the Min Elevation object.
     *
     * @return double Minimum value in the raster.
     * 
     * @author Ryan Wagster <rywa2447@colorado.edu>
     */
    inline double DemHandler::getMinElevation() const noexcept
    {
        return elevationMinMax[0];
    }

    /**
     * @brief Get the Max Elevation object.
     *
     * @return double Maximum value in the raster.
     * 
     * @author Ryan Wagster <rywa2447@colorado.edu>
     */
    inline double DemHandler::getMaxElevation() const noexcept
    {
        return elevationMinMax[1];
    }
#endif
}

#pragma once
#include <gdal_priv.h>
#include <vector>
#include <utility>

/**
 * @brief Namespace for CU Boulder Mars Elevation Model Pathfinding Algorithm.
 *
 */
namespace mempa
{
    /**
     * @brief Class to hold DEM interactions for CU Boulder MEMPA simulation.
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     */
    class DemHandler
    {
    private:
        const char *const pszFilename;                     /* Filepath to the input DEM.tif file. */
        static inline constexpr int ELEVATION_BAND = 1;    /* For a DEM, the Elevation Band should be at indice 1. For a slope raster, Band 1 is used for slope values. */
        static inline constexpr int MINMAX_SIZE = 2;       /* Size of the array to hold min and max raster values. */
        static inline constexpr int GEOTRANSFORM_SIZE = 6; /* GDAL Geotransforms are sets of 6 coefficients. */
        static inline constexpr double DEM_180 = 180.0;    /* Used to convert degrees to meters for spatial resolution. Only used for DEM data. */
        GDALDatasetUniquePtr poDataset;                    /* Pointer to the GDAL Dataset. Not actually a pointer. */
        GDALRasterBand *poBand;                            /* Pointer to the first band of the raster. */
        double elevationMinMax[MINMAX_SIZE];               /* Minimum: Index 0, Maximum: Index 1 */
        double adfGeoTransform[GEOTRANSFORM_SIZE];         /* Array to store all Geotransform values. */
        const char *poProjection;                          /* Name of CRS projection used by the raster. */
        OGRSpatialReference CRS;                           /* Coordinate Reference System of the raster. */

    protected:
        /* DemHandler is not designed to be subclassed. */

    public:
        explicit DemHandler(const char *pszFilename);
        std::vector<std::vector<float>> readSquareChunk(std::pair<int, int> imgCoordinate, int radius) const;
        std::vector<std::vector<float>> readCircleChunk(std::pair<int, int> imgCoordinate, int radius) const;
        std::vector<std::vector<float>> readRectangleChunk(std::pair<std::pair<int, int>, std::pair<int, int>> imgCoordinates, int radius) const;
        std::pair<int, int> transformCoordinates(std::pair<double, double> geoCoordinate) const noexcept;
        double getImageResolution() const;

        /**
         * @brief Get the Min Elevation object.
         *
         * @return double Minimum value in the raster.
         */
        inline double getMinElevation() const noexcept
        {
            return elevationMinMax[0];
        }

        /**
         * @brief Get the Max Elevation object.
         *
         * @return double Maximum value in the raster.
         */
        inline double getMaxElevation() const noexcept
        {
            return elevationMinMax[1];
        }

        /**
         * @brief Get the total X size of the raster.
         *
         * @return int Number of pixels per raster row.
         */
        inline int getXSize() const noexcept
        {
            return poBand->GetXSize();
        }

        /**
         * @brief Get the total Y size of the raster.
         *
         * @return int Number of pixels per raster column.
         */
        inline int getYSize() const noexcept
        {
            return poBand->GetYSize();
        }
    };
}

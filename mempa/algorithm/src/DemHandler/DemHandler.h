#pragma once
#include <vector>
#include <utility>
#include <gdal_priv.h>

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
    public:
        explicit DemHandler(const char *pszFilename);
        std::vector<std::vector<float>> readSquareChunk(std::pair<int, int> imgCoordinate, int radius) const;
        std::vector<std::vector<float>> readCircleChunk(std::pair<int, int> imgCoordinate, int radius) const;
        std::vector<std::vector<float>> readRectangleChunk(std::pair<std::pair<int, int>, std::pair<int, int>> imgCoordinates, int radius) const;
        std::pair<int, int> transformCoordinates(double xGeoCoordinate, const double yGeoCoordinate) const;
        double getImageResolution() const;

    private:
        const char *const pszFilename;              /* Filepath to the input DEM.tif file. */
        static constexpr int ELEVATION_BAND = 1;    /* For a DEM, the Elevation Band should be at indice 1. */
        static constexpr int GEOTRANSFORM_SIZE = 6; /* GDAL Geotransforms are sets of 6 coefficients. */
        static constexpr double DEM_180 = 180.0;    /* Used to convert degrees to meters for spatial resolution. */

    protected:
        GDALDatasetUniquePtr poDataset;            /* Pointer to the GDAL Dataset. Not actually a pointer. */
        GDALRasterBand *poBand;                    /* Pointer to the Elevation Values Band. */
        const char *poProjection;                  /* Name of CRS projection used by the DEM. */
        OGRSpatialReference CRS;                   /* Coordinate Reference System of the DEM. */
        double adfGeoTransform[GEOTRANSFORM_SIZE]; /* Array to store all Geotransform values. */
    };
}
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
        std::vector<std::vector<float>> readSquareChunk(const std::pair<int, int> &imgCoordinate, int radius) const;
        std::vector<std::vector<float>> readCircleChunk(const std::pair<int, int> &imgCoordinate, int radius) const;
        std::vector<std::vector<float>> readRectangleChunk(const std::pair<std::pair<int, int>, std::pair<int, int>> &imgCoordinates, int radius) const;
        std::pair<int, int> transformCoordinates(double xGeoCoordinate, double yGeoCoordinate) const;
        int getImageResolution() const;

    private:
        const char *const pszFilename;              /* Filepath to the input DEM.tif file. */
        GDALDatasetUniquePtr poDataset;             /* Pointer to the GDAL Dataset. */
        GDALRasterBand *poBand;                     /* Pointer to the Elevation Values Band. */
        static constexpr int ELEVATION_BAND = 1;    /* For a DEM, the Elevation Band should be at indice 1. */
        static constexpr int GEOTRANSFORM_SIZE = 6; /* GDAL Geotransforms are sets of 6 coefficients. */
        double adfGeoTransform[GEOTRANSFORM_SIZE];  /* Array to store all Geotransform values. */
    };
}
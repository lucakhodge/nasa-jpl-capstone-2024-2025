/* Local Header */
#include "DemHandler.hpp"

/* libgdal-dev */
#include <gdal_priv.h>

/* C++ Standard Libraries */
#include <limits>
#include <cmath>
#include <algorithm>
#include <vector>
#include <utility>

namespace mempa
{
    /**
     * @brief Construct a new Dem Handler:: Dem Handler object
     *
     * @param pszFilename Filepath to the DEM raster to be read.

     * @throws Failure to perform GDAL functions.
     *
     * @note For more information on GDAL Raster API, see: https://gdal.org/en/stable/tutorials/raster_api_tut.html
     */
    DemHandler::DemHandler(const char *const pszFilename)
        : pszFilename(pszFilename)
    {
        /* Initialize all GDAL operations. */
        GDALAllRegister();

        /* Load the filename into a new GDAL Dataset. */
        poDataset = GDALDatasetUniquePtr(GDALDataset::FromHandle(GDALOpen(this->pszFilename, GA_ReadOnly)));
        if (!poDataset)
        {
            throw std::runtime_error("DemHandler: GDALOpen() error");
        }

        /* Get the raster band of the dataset. DEM rasters always have one band. */
        poBand = poDataset->GetRasterBand(ELEVATION_BAND);
        if (!poBand)
        {
            throw std::runtime_error("DemHandler: GetRasterBand() error");
        }

#if DEMHANDLER_MINMAX
        /* Calculate the minimum and maximum values in the raster. First parameter is 0 for exact calculation, set to 1 for approximate. Causes a delay in runtime while statistics are calculated. */
        if (poBand->ComputeRasterMinMax(0, elevationMinMax) != CE_None)
        {
            throw std::runtime_error("DemHandler: ComputeRasterMinMax() error");
        }
#endif

        /* Load the GeoTransform metadata from the dataset. */
        if (poDataset->GetGeoTransform(adfGeoTransform) != CE_None)
        {
            throw std::runtime_error("DemHandler: GetGeoTransform() error");
        }

        /* Get the CRS projection reference from the dataset. */
        poProjection = poDataset->GetProjectionRef();
        if (poProjection == nullptr || poProjection[0] == '\0')
        {
            throw std::runtime_error("DemHandler: GetProjectionRef() error");
        }

        /* Initialize the OGRSpatialReference with the CRS Projection. */
        if (CRS.importFromWkt(poProjection) != OGRERR_NONE)
        {
            throw std::runtime_error("DemHandler: importFromWkt() error");
        }
    }

    /**
     * @brief Creates a square chunk from a single coordinate point. The input coordinate will be the centerpoint of the chunk.
     *
     * @param imgCoordinate A pair of integer image coordinates.
     * @param buffer The value used for the general size of the output chunk of elevation data.
     * @param relativeCoordinate Optional parameter to reflect the local vector index of the image coordinate.
     *
     * @return std::vector<std::vector<float>> Elevation values.
     *
     * @throws Failure to allocate memory or read raster values.
     */
    std::vector<std::vector<float>> DemHandler::readSquareChunk(const std::pair<int, int> imgCoordinate, const int buffer, std::pair<int, int> *relativeCoordinate) const
    {
        /* Get the X and Y coordiantes from the pair. */
        const int xCenter = imgCoordinate.first;  /* Input X ordinate. */
        const int yCenter = imgCoordinate.second; /* Input Y ordinate. */

        /* Ensure the area to read is within raster bounds. */
        const int xOff = std::max(0, xCenter - buffer);                      /* Left X offset. */
        const int yOff = std::max(0, yCenter - buffer);                      /* Bottom Y offset. */
        const int xEnd = std::min(poBand->GetXSize(), xCenter + buffer + 1); /* Right X offset. */
        const int yEnd = std::min(poBand->GetYSize(), yCenter + buffer + 1); /* Top Y offset. */

        /* Get size of in-boundary chunk to read. */
        const int xSize = xEnd - xOff; /* Total X size to be read. */
        const int ySize = yEnd - yOff; /* Total Y size to be read. */

        /* Read raster data into 1D vector of floats. */
        std::vector<float> vScanline(xSize * ySize); /* Vector to hold RasterIO read. */
        if (poBand->RasterIO(GF_Read, xOff, yOff, xSize, ySize, vScanline.data(), xSize, ySize, GDT_Float32, 0, 0) != CE_None)
        {
            throw std::runtime_error("readSquareChunk: RasterIO() error");
        }

        /* Build a 2D vector of vectors of floats from the 1D vector. */
        std::vector<std::vector<float>> rasterVector(ySize, std::vector<float>(xSize)); /* Vector to hold RasterIO read in 2D indexing. */
        for (int rowIndex = 0; rowIndex < ySize; ++rowIndex)                            /* Current row to be copied. */
        {
            std::copy(vScanline.begin() + rowIndex * xSize, vScanline.begin() + (rowIndex + 1) * xSize, rasterVector[rowIndex].begin());
        }

        /* Modify the input pair to hold the input coordinates as vector indices. */
        if (relativeCoordinate != nullptr)
        {
            *relativeCoordinate = std::pair<int, int>(xCenter - xOff, yCenter - yOff);
        }

        /* Return 2D vector of floats. */
        return rasterVector;
    }

    /**
     * @brief Creates a circular chunk from a single coordinate point. The input coordinate will be the centerpoint of the chunk.
     *
     * @details The corner values of the initial square will be overwritten with NAN.
     *
     * @param imgCoordinate A pair of integer image coordinates.
     * @param radius The value used for the general size of the output chunk of elevation data.
     * @param relativeCoordinate Optional parameter to reflect the local vector index of the image coordinate.
     *
     * @return std::vector<std::vector<float>> Elevation values.
     *
     * @throw Failure to read raster values.
     */
    std::vector<std::vector<float>> DemHandler::readCircleChunk(const std::pair<int, int> imgCoordinate, const int radius, std::pair<int, int> *relativeCoordinate) const
    {
        /* Ensure that the initial 2D vector of floats is valid. */
        std::vector<std::vector<float>> rasterVector = readSquareChunk(imgCoordinate, radius, relativeCoordinate);
        if (rasterVector.empty())
        {
            throw std::runtime_error("readCircleChunk: readSquareChunk() error");
        }

        /* Set up values for distance calculations. */
        const int xVec = static_cast<int>(rasterVector.front().size()); /* Size of each row. */
        const int yVec = static_cast<int>(rasterVector.size());         /* Size of each column. */
        const int xCenter = xVec / 2;                                   /* X center ordinate, assuming square chunk. */
        const int yCenter = yVec / 2;                                   /* Y center ordinate, assuming square chunk. */
        const int radiusSquared = radius * radius;                      /* Hypotenuse for pythagorean theorem. */

        /* For each coordinate in the chunk, change to not a number float if outside the radius. */
        for (int row = 0; row < yVec; ++row)
        {
            for (int col = 0; col < xVec; ++col)
            {
                int xDistance = (col - xCenter) * (col - xCenter); /* First length for pythagorean theorem. */
                int yDistance = (row - yCenter) * (row - yCenter); /* Second length for pythagorean theorem. */
                if (xDistance + yDistance > radiusSquared)
                {
                    rasterVector[row][col] = std::numeric_limits<float>::quiet_NaN();
                }
            }
        }

        /* Return 2D vector of floats with not a number float corners to resemble a circle of values. */
        return rasterVector;
    }

    /**
     * @brief Creates a rectangular chunk from a pair of coordinate points. Values sorrounding both coordinates as well as every value in between them will make up the chunk.
     *
     * @details The coordinates will not necessarily be the center points of the chunk, but should be around opposing corners buffered by the radius.
     *
     * @param imgCoordinates A pair of pairs of integer coordinates.
     * @param buffer The value used for the general size of the output chunk of elevation data.
     * @param relativeCoordinates Optional parameter to reflect the local vector index of each image coordinate.
     *
     * @return std::vector<std::vector<float>> Elevation values.
     *
     * @throws Failure to allocate memory or read raster values.
     */
    std::vector<std::vector<float>> DemHandler::readRectangleChunk(const std::pair<std::pair<int, int>, std::pair<int, int>> imgCoordinates, const int buffer, std::pair<std::pair<int, int>, std::pair<int, int>> *relativeCoordinates) const
    {
        /* Get X and Y coordinates from both pairs. */
        const int xCenter1 = imgCoordinates.first.first;   /* First coordinate X. */
        const int yCenter1 = imgCoordinates.first.second;  /* First coordinate Y.*/
        const int xCenter2 = imgCoordinates.second.first;  /* Second coordinate X. */
        const int yCenter2 = imgCoordinates.second.second; /* Second coordinate Y. */

        /* Build boundaries from raster size and offsets from both coordinates. */
        const int xOff = std::max(0, std::min(xCenter1, xCenter2) - buffer);                      /* Left X offset. */
        const int yOff = std::max(0, std::min(yCenter1, yCenter2) - buffer);                      /* Bottom Y offset. */
        const int xEnd = std::min(poBand->GetXSize(), std::max(xCenter1, xCenter2) + buffer + 1); /* Right X offset. */
        const int yEnd = std::min(poBand->GetYSize(), std::max(yCenter1, yCenter2) + buffer + 1); /* Top Y offset. */

        /* Get size of in-boundary chunk to read. */
        const int xSize = xEnd - xOff; /* Total X size to be read. */
        const int ySize = yEnd - yOff; /* Total Y size to be read. */

        /* Read raster data within our chunk into the 1D vector. */
        std::vector<float> vScanline(xSize * ySize); /* Vector to hold RasterIO read. */
        if (poBand->RasterIO(GF_Read, xOff, yOff, xSize, ySize, vScanline.data(), xSize, ySize, GDT_Float32, 0, 0) != CE_None)
        {
            throw std::runtime_error("readRectangleChunk: RasterIO() error");
        }

        /* Build a 2D vector of vectors of floats from the 1D vector. */
        std::vector<std::vector<float>> rasterVector(ySize, std::vector<float>(xSize)); /* Vector to hold RasterIO read in 2D indexing. */
        for (int rowIndex = 0; rowIndex < ySize; ++rowIndex)                            /* Current row to be copied. */
        {
            std::copy(vScanline.begin() + rowIndex * xSize, vScanline.begin() + (rowIndex + 1) * xSize, rasterVector[rowIndex].begin());
        }

        /* Modify the input pair to hold the input coordinates as vector indices. */
        if (relativeCoordinates != nullptr)
        {
            relativeCoordinates->first = std::pair<int, int>(xCenter1 - xOff, yCenter1 - yOff);
            relativeCoordinates->second = std::pair<int, int>(xCenter2 - xOff, yCenter2 - yOff);
        }

        /* Return 2D vector of floats. */
        return rasterVector;
    }

    /**
     * @brief Transform a latitide, longitude (x, y) pair of (double) geographical coordinates into the corresponding pair of (integer) image pixel (x, y) coordinates.
     *
     * @param geoCoordinate Coordinate pair of doubles.
     *
     * @return std::pair<int, int> Pixel-based image coordinates.
     *
     * @note For more information on Geotransform, see: https://gdal.org/en/stable/tutorials/geotransforms_tut.html
     */
    std::pair<int, int> DemHandler::transformCoordinates(const std::pair<double, double> geoCoordinate) const noexcept
    {
        const int xPixelCoordinate = static_cast<int>((geoCoordinate.first - adfGeoTransform[0]) / adfGeoTransform[1]);  /* Longitude ordinate minus upper-left pixel X coordinate divided by pixel width. */
        const int yPixelCoordinate = static_cast<int>((geoCoordinate.second - adfGeoTransform[3]) / adfGeoTransform[5]); /* Latitude ordinate minus upper left pixel Y coordinate divided by pixel height. */
        return std::pair<int, int>(xPixelCoordinate, yPixelCoordinate);
    }

    /**
     * @brief Transform a pair of integer coordinates into the corresponding latitide, longitude (x, y) pair of double geographical coordinates.
     *
     * @param imgCoordinate Coordinate pair of integers.
     * @return std::pair<double, double> Georeferenced coordinates.
     */
    std::pair<double, double> DemHandler::revertCoordinates(const std::pair<int, int> imgCoordinate) const noexcept
    {
        const double xGeoCoordinate = (static_cast<double>(imgCoordinate.first) * adfGeoTransform[1]) + adfGeoTransform[0];
        const double yGeoCoordinate = (static_cast<double>(imgCoordinate.second) * adfGeoTransform[5]) + adfGeoTransform[3];
        return std::pair<double, double>(xGeoCoordinate, yGeoCoordinate);
    }

    /**
     * @brief Gets the spatial resolution of the raster.
     *
     * @return double Pixel resolution in meters.
     *
     * @throw Non-Square pixels (inequal height and width) are invalid.
     */
    double DemHandler::getImageResolution() const
    {
        /* Get the west-east pixel resolution and the north-south pixel resolution. */
        const double pixelWidth = adfGeoTransform[1];  /* West-East pixel resolution. */
        const double pixelHeight = adfGeoTransform[5]; /* North-South pixel resolution. */

        /* Pixels must always be square. */
        if (pixelWidth != std::abs(pixelHeight))
        {
            throw std::runtime_error("getImageResolution: non-square resolution");
        }

        /* Return the spatial resolution in meters. Modify based on the CRS. */
        double metersResolution = pixelWidth; /* Pixel resolution in meters. */
        if (CRS.IsGeographic())
        {
            /* Convert from degrees to meters. */
            const double metersPerDegree = (M_PI * CRS.GetSemiMajor()) / DEM_180; /* Converstion constant for degrees to meters. Pi times the CRS's semi-major axis divided by 180. */
            metersResolution *= metersPerDegree;
        }

        /* Return the resolution. */
        return metersResolution;
    }
}

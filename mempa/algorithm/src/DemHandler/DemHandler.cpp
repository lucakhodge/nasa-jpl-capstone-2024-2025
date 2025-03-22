#include "DemHandler.h"
#include <algorithm>
#include <cassert>
#include <cmath>

namespace mempa
{
    /**
     * @brief Construct a new Dem Handler:: Dem Handler object
     *
     * @param pszFilename Filepath to the DEM raster to be read.
     *
     * @throws Failure to create DGAL dataset, read the elevation raster band, or get geotransform.
     *
     * @note For more information on GDAL Raster API, see: https://gdal.org/en/stable/tutorials/raster_api_tut.html
     */
    DemHandler::DemHandler(const char *const pszFilename) : pszFilename(pszFilename)
    {
        /* Initialize all GDAL operations. */
        GDALAllRegister();

        /* Load the filename into a new GDAL Dataset. */
        poDataset = GDALDatasetUniquePtr(GDALDataset::FromHandle(GDALOpen(pszFilename, GA_ReadOnly)));
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

        /* Load the GeoTransform metadata from the dataset. */
        if (poDataset->GetGeoTransform(adfGeoTransform) != CE_None)
        {
            throw std::runtime_error("DemHandler: GetGeoTransform() error");
        }

        /* Get the CRS projection reference from the dataset. */
        poProjection = poDataset->GetProjectionRef();
        if (!poProjection || *poProjection == '\0')
        {
            throw std::runtime_error("DemHandler: GetProjectionRef() error");
        }
    }

    /**
     * @brief Creates a square chunk from a single coordinate point. The input coordinate will be the centerpoint of the chunk.
     *
     * @param imgCoordinate A pair of integer image coordinates.
     * @param radius The value used for the general size of the output chunk of elevation data.
     *
     * @return std::vector<std::vector<float>> Elevation values.
     *
     * @throws Failure to allocate memory or read raster values.
     */
    std::vector<std::vector<float>> DemHandler::readSquareChunk(const std::pair<int, int> imgCoordinate, const int radius) const
    {
        /* Get the X and Y coordiantes from the pair. */
        const int xCenter = imgCoordinate.first;
        const int yCenter = imgCoordinate.second;

        /* Ensure the area to read is within raster bounds. */
        const int xOff = std::max(0, xCenter - radius);
        const int yOff = std::max(0, yCenter - radius);
        const int xEnd = std::min(poBand->GetXSize(), xCenter + radius + 1);
        const int yEnd = std::min(poBand->GetYSize(), yCenter + radius + 1);

        /* Get size of in-boundary chunk to read. */
        const int xSize = xEnd - xOff;
        const int ySize = yEnd - yOff;

        /* Allocate memory and read raster data into 1D pointer to array of floats. */
        float *pafScanline = reinterpret_cast<float *>(CPLMalloc(sizeof(float) * xSize * ySize));
        if (!pafScanline)
        {
            throw std::runtime_error("readSquareChunk: CPLMalloc() error");
        }

        /* Read raster data within our chunk into the 1D array. */
        if (poBand->RasterIO(GF_Read, xOff, yOff, xSize, ySize, pafScanline, xSize, ySize, GDT_Float32, 0, 0) != CE_None)
        {
            CPLFree(pafScanline);
            throw std::runtime_error("readSquareChunk: RasterIO() error");
        }

        /* Build a 2D vector of vectors of floats from the 1D array. */
        std::vector<std::vector<float>> rasterVector(ySize, std::vector<float>(xSize));
        for (int yVec = 0; yVec < ySize; ++yVec)
        {
            for (int xVec = 0; xVec < xSize; ++xVec)
            {
                rasterVector[yVec][xVec] = pafScanline[yVec * xSize + xVec];
            }
        }

        /* Free memory and return the 2D vector. */
        CPLFree(pafScanline);
        return rasterVector;
    }

    /**
     * @brief Creates a circular chunk from a single coordinate point. The input coordinate will be the centerpoint of the chunk.
     *
     * @details The corner values of the initial square will be overwritten with NaN.
     *
     * @param imgCoordinate A pair of integer image coordinates.
     * @param radius The value used for the general size of the output chunk of elevation data.
     *
     * @return std::vector<std::vector<float>> Elevation values.
     *
     * @throw Failure to read raster values.
     */
    std::vector<std::vector<float>> DemHandler::readCircleChunk(const std::pair<int, int> imgCoordinate, const int radius) const
    {
        /* Ensure that the initial 2D vector of floats is valid. */
        std::vector<std::vector<float>> rasterVector = readSquareChunk(imgCoordinate, radius);
        if (rasterVector.empty())
        {
            throw std::runtime_error("readCircleChunk: readSquareChunk() error");
        }

        /* Set up values for distance calculations. */
        const int xVec = static_cast<int>(rasterVector.front().size());
        const int yVec = static_cast<int>(rasterVector.size());
        const int xCenter = xVec / 2;
        const int yCenter = yVec / 2;

        /* For each coordinate in the chunk, change to not a number float if outside the radius. */
        for (int row = 0; row < xVec; ++row)
        {
            for (int col = 0; col < yVec; ++col)
            {
                int xDistance = (col - xCenter) * (col - xCenter);
                int yDistance = (row - yCenter) * (row - yCenter);
                if (xDistance + yDistance > radius * radius)
                {
                    rasterVector[row][col] = std::nanf("");
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
     * @param radius The value used for the general size of the output chunk of elevation data.
     *
     * @return std::vector<std::vector<float>> Elevation values.
     *
     * @throws Failure to allocate memory or read raster values.
     */
    std::vector<std::vector<float>> DemHandler::readRectangleChunk(const std::pair<std::pair<int, int>, std::pair<int, int>> imgCoordinates, const int radius) const
    {
        /* Get X and Y coordinates from both pairs. */
        const int xCenter1 = imgCoordinates.first.first;
        const int yCenter1 = imgCoordinates.first.second;
        const int xCenter2 = imgCoordinates.second.first;
        const int yCenter2 = imgCoordinates.second.second;

        /* Build boundaries from raster size and offsets from both coordinates. */
        const int xOff = std::max(0, std::min(xCenter1, xCenter2) - radius);
        const int yOff = std::max(0, std::min(yCenter1, yCenter2) - radius);
        const int xEnd = std::min(poBand->GetXSize(), std::max(xCenter1, xCenter2) + radius + 1);
        const int yEnd = std::min(poBand->GetYSize(), std::max(yCenter1, yCenter2) + radius + 1);

        /* Get size of in-boundary chunk to read. */
        const int xSize = xEnd - xOff;
        const int ySize = yEnd - yOff;

        /* Allocate memory and read raster data into 1D pointer to array of floats. */
        float *pafScanline = reinterpret_cast<float *>(CPLMalloc(sizeof(float) * xSize * ySize));
        if (!pafScanline)
        {
            throw std::runtime_error("readRectangleChunk: CPLMalloc() error");
        }

        /* Read raster data within our chunk into the 1D array. */
        if (poBand->RasterIO(GF_Read, xOff, yOff, xSize, ySize, pafScanline, xSize, ySize, GDT_Float32, 0, 0) != CE_None)
        {
            CPLFree(pafScanline);
            throw std::runtime_error("readRectangleChunk: RasterIO() error");
        }

        /* Build a 2D vector of vectors of floats from the 1D array. */
        std::vector<std::vector<float>> rasterVector(ySize, std::vector<float>(xSize));
        for (int yVec = 0; yVec < ySize; ++yVec)
        {
            for (int xVec = 0; xVec < xSize; ++xVec)
            {
                rasterVector[yVec][xVec] = pafScanline[yVec * xSize + xVec];
            }
        }

        /* Free memory and return the 2D vector. */
        CPLFree(pafScanline);
        return rasterVector;
    }

    /**
     * @brief Transform a latitide, longitude (x, y) pair of (double) geographical coordinates into the corresponding pair of (integer) image pixel (x, y) coordiantes.
     *
     * @param xGeoCoordinate The longitude value.
     * @param yGeoCoordinate The latitude value.
     *
     * @return std::pair<int, int> Pixel-based image coordinates.
     *
     * @note For more information on Geotransform, see: https://gdal.org/en/stable/tutorials/geotransforms_tut.html
     */
    std::pair<int, int> DemHandler::transformCoordinates(const double xGeoCoordinate, const double yGeoCoordinate) const
    {
        const int xPixelCoordinate = static_cast<int>((xGeoCoordinate - adfGeoTransform[0]) / adfGeoTransform[1]);
        const int yPixelCoordinate = static_cast<int>((yGeoCoordinate - adfGeoTransform[3]) / adfGeoTransform[5]);
        return std::make_pair(xPixelCoordinate, yPixelCoordinate);
    }

    /**
     * @brief Gets the spatial resolution of the raster.
     *
     * @return int Pixel resolution in meters.
     *
     * @throw Non-Square pixels (inequal height and width) are invalid.
     */
    int DemHandler::getImageResolution() const
    {
        /* Cast the west-east pixel resolution and the north-south pixel resolution to intgers. */
        const int pixelWidth = static_cast<int>(adfGeoTransform[1]);
        const int pixelHeight = static_cast<int>(std::abs(adfGeoTransform[5]));

        /* Pixels must always be square. */
        if (pixelWidth != pixelHeight)
        {
            throw std::runtime_error("getImageResolution: non-square resolution");
        }

        /* Return the resolution. */
        return pixelWidth;
    }
}

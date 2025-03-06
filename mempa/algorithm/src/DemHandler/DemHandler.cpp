#include "DemHandler.h"
#include <algorithm>
#include <cmath>
namespace mempa
{
    /**
     * @brief Construct a new Dem Handler:: Dem Handler object
     *
     * @param pszFilename Filepath to the DEM raster to be read.
     *
     * @note For more information on GDAL Raster API, see: https://gdal.org/en/stable/tutorials/raster_api_tut.html
     */
    DemHandler::DemHandler(const char *const pszFilename) : pszFilename(pszFilename)
    {
        GDALAllRegister();
        poDataset = GDALDatasetUniquePtr(GDALDataset::FromHandle(GDALOpen(pszFilename, GA_ReadOnly)));
        if (!poDataset)
        {
            throw std::runtime_error("Failure to create GDAL dataset.");
        }
        poBand = poDataset->GetRasterBand(ELEVATION_BAND);
        if (!poBand)
        {
            throw std::runtime_error("Failure to get raster band.");
        }
        if (poDataset->GetGeoTransform(adfGeoTransform) != CE_None)
        {
            throw std::runtime_error("Failure to get geotransform.");
        }
    }

    /**
     * @brief Creates a square chunk from a single coordinate point. The input coordinate will be the centerpoint of the chunk.
     *
     * @param imgCoordinate A pair of integer image coordinates.
     * @param radius The value used for the general size of the output chunk of elevation data.
     * 
     * @return std::vector<std::vector<float>>
     */
    std::vector<std::vector<float>> DemHandler::readSquareChunk(const std::pair<int, int> &imgCoordinate, const int radius) const
    {
        const int xCenter = imgCoordinate.first;
        const int yCenter = imgCoordinate.second;
        const int xOff = std::max(0, xCenter - radius);
        const int yOff = std::max(0, yCenter - radius);
        const int xEnd = std::min(poBand->GetXSize(), xCenter + radius + 1);
        const int yEnd = std::min(poBand->GetYSize(), yCenter + radius + 1);
        const int xSize = xEnd - xOff;
        const int ySize = yEnd - yOff;
        float *pafScanline = reinterpret_cast<float *>(CPLMalloc(sizeof(float) * xSize * ySize));
        if (!pafScanline)
        {
            throw std::runtime_error("Failed to allocate memory.");
        }
        if (poBand->RasterIO(GF_Read, xOff, yOff, xSize, ySize, pafScanline, xSize, ySize, GDT_Float32, 0, 0) != CE_None)
        {
            CPLFree(pafScanline);
            throw std::runtime_error("Failure to read raster values.");
        }
        std::vector<std::vector<float>> rasterVector(ySize, std::vector<float>(xSize));
        for (int yVec = 0; yVec < ySize; ++yVec)
        {
            for (int xVec = 0; xVec < xSize; ++xVec)
            {
                rasterVector[yVec][xVec] = pafScanline[yVec * xSize + xVec];
            }
        }
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
     * @return std::vector<std::vector<float>>
     */
    std::vector<std::vector<float>> DemHandler::readCircleChunk(const std::pair<int, int> &imgCoordinate, const int radius) const
    {
        std::vector<std::vector<float>> rasterVector = readSquareChunk(imgCoordinate, radius);
        if (rasterVector.empty())
        {
            throw std::runtime_error("Cannot read empty chunk.");
        }
        const int xVec = rasterVector.front().size();
        const int yVec = rasterVector.size();
        const int xCenter = xVec / 2;
        const int yCenter = yVec / 2;
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
     * @return std::vector<std::vector<float>>
     */
    std::vector<std::vector<float>> DemHandler::readRectangleChunk(const std::pair<std::pair<int, int>, std::pair<int, int>> &imgCoordinates, const int radius) const
    {
        const std::pair<int, int> imgCoordinate1 = transformCoordinates(imgCoordinates.first.first, imgCoordinates.first.second);
        const std::pair<int, int> imgCoordinate2 = transformCoordinates(imgCoordinates.second.first, imgCoordinates.second.second);
        const int xCenter1 = imgCoordinate1.first;
        const int yCenter1 = imgCoordinate1.second;
        const int xCenter2 = imgCoordinate2.first;
        const int yCenter2 = imgCoordinate2.second;
        const int xOff = std::max(0, std::min(xCenter1, xCenter2) - radius);
        const int yOff = std::max(0, std::min(yCenter1, yCenter2) - radius);
        const int xEnd = std::min(poBand->GetXSize(), std::max(xCenter1, xCenter2) + radius + 1);
        const int yEnd = std::min(poBand->GetYSize(), std::max(yCenter1, yCenter2) + radius + 1);
        const int xSize = xEnd - xOff;
        const int ySize = yEnd - yOff;
        float *pafScanline = reinterpret_cast<float *>(CPLMalloc(sizeof(float) * xSize * ySize));
        if (!pafScanline)
        {
            throw std::runtime_error("Failed to allocate memory.");
        }
        if (poBand->RasterIO(GF_Read, xOff, yOff, xSize, ySize, pafScanline, xSize, ySize, GDT_Float32, 0, 0) != CE_None)
        {
            CPLFree(pafScanline);
            throw std::runtime_error("Failure to read raster values.");
        }
        std::vector<std::vector<float>> rasterVector(ySize, std::vector<float>(xSize));
        for (int yVec = 0; yVec < ySize; ++yVec)
        {
            for (int xVec = 0; xVec < xSize; ++xVec)
            {
                rasterVector[yVec][xVec] = pafScanline[yVec * xSize + xVec];
            }
        }
        CPLFree(pafScanline);
        return rasterVector;
    }

    /**
     * @brief Transform a latitide, longitude (x, y) pair of (double) geographical coordinates into the corresponding pair of (integer) image pixel (x, y) coordiantes.
     *
     * @param xGeoCoordinate The longitude value.
     * @param yGeoCoordinate The latitude value.
     *
     * @return std::pair<int, int>
     *
     * @note For more information on Geotransform, see: https://gdal.org/en/stable/tutorials/geotransforms_tut.html
     */
    std::pair<int, int> DemHandler::transformCoordinates(const double xGeoCoordinate, const double yGeoCoordinate) const
    {
        const int xPixelCoordinate = static_cast<int>((xGeoCoordinate - adfGeoTransform[0]) / adfGeoTransform[1]);
        const int yPixelCoordinate = static_cast<int>((yGeoCoordinate - adfGeoTransform[3]) / adfGeoTransform[5]);
        return std::make_pair(xPixelCoordinate, yPixelCoordinate);
    }
}

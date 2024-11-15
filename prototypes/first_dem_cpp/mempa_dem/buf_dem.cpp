/* Capstone Project Libraries */
#include "buf_dem.h"

/**
 * @brief Constructs a new @c MEMPA::DEM object.
 *
 * @details
 * This constructor initializes a @ref BUF_DEM object by setting the input GeoTIFF file path and the output directory path.
 * It performs validation on the output directory to ensure that it exists and is a valid directory.
 *
 * @param ifp The file path to the input DEM GeoTIFF image. This file contains the Digital Elevation Model data
 *            to be processed.
 * @param ofp The output directory where new files, generated from the DEM data (such as chunked DEM data),
 *            will be saved. If the directory does not exist or is invalid, an exception is thrown.
 *
 * @throws std::invalid_argument Thrown if the provided output file path is an empty string.
 * @throws std::filesystem::filesystem_error Thrown if the output directory does not exist or is not a valid directory.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-10-01
 */
MEMPA::BUF_DEM::BUF_DEM(const std::string_view ifp, const std::string_view ofp) : dem_fp(ifp), out_fp(ofp)
{
    /*
    Output file path error checking:
    - If the output file path is empty.
    - If the output file path does not exist.
    - If the output file path is not a directory.
    */
    if (ofp.empty())
    {
        throw std::invalid_argument("Output file path is empty.");
    }

    if (!std::filesystem::exists(out_fp))
    {
        throw std::filesystem::filesystem_error("Output path does not exist: " + std::string(ofp),
                                                std::make_error_code(std::errc::no_such_file_or_directory));
    }

    if (!std::filesystem::is_directory(out_fp))
    {
        throw std::filesystem::filesystem_error("Output path is not a directory: " + std::string(ofp),
                                                std::make_error_code(std::errc::not_a_directory));
    }

    /*
    Input file path error checking:
    - If the input file path is empty.
    - If the input file path does not exist.
    - If the input file path is a directory.
    */
    if (ifp.empty())
    {
        throw std::invalid_argument("Input DEM file path is empty.");
    }

    if (!std::filesystem::exists(dem_fp))
    {
        throw std::filesystem::filesystem_error("Input DEM file does not exist: " + std::string(ifp),
                                                std::make_error_code(std::errc::no_such_file_or_directory));
    }

    if (std::filesystem::is_directory(dem_fp))
    {
        throw std::filesystem::filesystem_error("Input DEM path is a directory: " + std::string(ifp),
                                                std::make_error_code(std::errc::is_a_directory));
    }

    GDALAllRegister();
    dem_dataset = static_cast<GDALDataset *>(GDALOpen(dem_fp.c_str(), GA_ReadOnly));
    if (!dem_dataset)
    {
        const char *GDAL_errMsg = CPLGetLastErrorMsg();
        throw std::runtime_error("Failed to open DEM file: " + dem_fp.string() + ". Error: " + (GDAL_errMsg ? GDAL_errMsg : "Unknown error"));
    }

    xSize = dem_dataset->GetRasterXSize();
    ySize = dem_dataset->GetRasterYSize();
    rasterCount = dem_dataset->GetRasterCount();
}

/**
 * @brief Destroys the @c MEMPA::DEM object.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-10-01
 */
MEMPA::BUF_DEM::~BUF_DEM()
{
    if (dem_dataset)
    {
        GDALClose(dem_dataset);
        dem_dataset = nullptr;
    }
}

/**
 * @brief Getter for the @c dem_dataset pointer to a @c BUF_DEM GDALDataset.
 *
 * @return GDALDataset* pointer to the loaded GDAL dataset.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-10-01
 */
GDALDataset *MEMPA::BUF_DEM::dem_get()
{
    return dem_dataset;
}

void MEMPA::BUF_DEM::dem_chunk()
{
    // NEED to implement some sort of data structure to
    // have adjacent chunks point to another to allow rover to traverse
    // NEED to return pointer to starting tile in Chunk dataset
    dem_dataset = dem_grab(); // dem_dataset is already the pointer to the dataset. getting itself unnecesary

    if (dem_dataset == NULL)
    {
        std::cerr << "Error opening dataset: " << inputFilename << std::endl; // should be a throw
        return;
    }

    // Get dimensions of DEM
    // width
    int cols = dem_dataset->GetRasterXSize(); // obsolete gets here from new private variables
    // height
    int rows = dem_dataset->GetRasterYSize();
    // bands
    int bands = dem_dataset->GetRasterCount();

        GDALDriver *Driver = GetGDALDriverManager()->GetDriverByName("GTiff");

    int chunkCount = 1;

    for (int i = 0; i < rows; i += chunkSize)
    {
        for (int j = 0; j < cols; j += chunkSize)
        {
            int chunkWidth = std::min(chunkSize, cols - j);
            int chunkHeight = std::min(chunkSize, rows - i);

            // Create chunk filename with chunk number
            std::string chunkFilename = std::string(outputFolder) + "/chunk_" + std::to_string(chunkCount++) + ".tif";
            GDALDataset *ChunkDataset = Driver->Create(chunkFilename.c_str(), chunkWidth, chunkHeight, bands, GDT_Float32, nullptr);

            if (ChunkDataset == nullptr)
            {
                std::cerr << "Failed to make a chunk file for " << chunkCount << std::endl; // cerr is part of the iostream header, which we aren't including in buf_dem. throw errors instead, which get caught in buf_main.
                continue;
            }

            // Loop through each band in the dataset
            for (int b = 1; b <= bands; ++b)
            {
                // buffer for chunk data
                float *buffer = new float[chunkWidth * chunkHeight];

                // DEM data into buffer
                dem_dataset->GetRasterBand(b)->RasterIO(GF_Read, j, i, chunkWidth, chunkHeight, buffer, chunkWidth, chunkHeight, GDT_Float32, 0, 0);

                // buffer into chunk
                ChunkDataset->GetRasterBand(b)->RasterIO(GF_Write, 0, 0, chunkWidth, chunkHeight, buffer, chunkWidth, chunkHeight, GDT_Float32, 0, 0);

                // free buffer memory
                delete[] buffer;
            }
        }
    }
}

void MEMPA::BUF_DEM::dem_safeid()
{
}

void MEMPA::BUF_DEM::dem_filter()
{
}

void MEMPA::BUF_DEM::dem_infostream()
{
}

void MEMPA::BUF_DEM::dem_getdata()
{
}
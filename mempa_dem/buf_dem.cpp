/* Capstone Project Libraries */
#include "buf_dem.h"

/**
 * @brief Constructs a new @c MEMPA::DEM object for managing DEM data.
 *
 * @details
 * This constructor initializes a @ref BUF_DEM object by setting the input GeoTIFF file path and the output directory path.
 * It performs validation on the output directory to ensure that it exists and is a valid directory.
 * The @c dem_dataset is initialized as a @c nullptr and must be loaded through the @ref dem_load function.
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
MEMPA::BUF_DEM::BUF_DEM(std::string ifp, std::string ofp) : dem_fp(ifp), out_fp(ofp), dem_dataset(nullptr)
{
    if (ofp.empty())
    {
        throw std::invalid_argument("Output file path is empty.");
    }
    if (!std::filesystem::exists(out_fp))
    {
        throw std::filesystem::filesystem_error("Output path does not exist: " + std::string(ofp), std::make_error_code(std::errc::no_such_file_or_directory));
    }
    if (!std::filesystem::is_directory(out_fp))
    {
        throw std::filesystem::filesystem_error("Output path is not a directory: " + std::string(ofp), std::make_error_code(std::errc::no_such_file_or_directory));
    }
}

/**
 * @brief Destroy the @c MEMPA::DEM object and sets the @c dem_dataset to @c nullptr .
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
 * @brief Loads the @c BUF_DEM dataset from the file path specified in @c dem_fp using GDAL.
 *
 * @details
 * This function attempts to open the DEM dataset in read-only mode via the GDALOpen function.
 *
 * @return GDALDataset* A pointer to the loaded GDAL dataset.
 *         - If successful, this pointer will point to the loaded dataset.
 *         - If the loading fails, an exception is thrown.
 *
 * @throws std::runtime_error Thrown if the dataset cannot be opened using GDAL.
 *
 * @note This function updates the @c dem_dataset member variable with the loaded dataset.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-10-01
 */
GDALDataset *MEMPA::BUF_DEM::dem_load()
{
    dem_dataset = static_cast<GDALDataset *>(GDALOpen(dem_fp.c_str(), GA_ReadOnly));
    if (!dem_dataset)
    {
        throw std::runtime_error("Failed to open input dataset: " + std::string(dem_fp));
    }
    return dem_dataset;
}

/**
 * @brief Getter for the @c dem_dataset pointer to a @c BUF_DEM GDALDataset.
 *
 * @details
 * This function returns the pointer to the dataset loaded in @ref dem_load function.
 *
 * @return GDALDataset* A pointer to the loaded GDAL dataset.
 *
 * @note If @c dem_load has not been run, this function should return a @c nullptr .
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-10-01
 */
GDALDataset *MEMPA::BUF_DEM::dem_grab()
{
    return dem_dataset;
}

void MEMPA::BUF_DEM::dem_chunk()
{
    // NEED to implement some sort of data structure to 
    // have adjacent chunks point to another to allow rover to traverse
    // NEED to return pointer to starting tile in Chunk dataset
    dem_dataset = dem_grab();

    if (dem_dataset == NULL) {
           std::cerr << "Error opening dataset: " << inputFilename << std::endl;
           return;
       }
    
    // Get dimensions of DEM
    // width
    int cols = dem_dataset->GetRasterXSize(); 
    // height
    int rows = dem_dataset->GetRasterYSize(); 
    // bands
    int bands = dem_dataset->GetRasterCount();

    GDALDriver *Driver = GetGDALDriverManager()->GetDriverByName("GTiff");

    int chunkCount = 1;

    for (int i = 0; i < rows; i += chunkSize) {
        for (int j = 0; j < cols; j += chunkSize) {
            int chunkWidth = std::min(chunkSize, cols - j);
            int chunkHeight = std::min(chunkSize, rows - i);

            // Create chunk filename with chunk number
            std::string chunkFilename = std::string(outputFolder) + "/chunk_" + std::to_string(chunkCount++) + ".tif";
            GDALDataset *ChunkDataset = Driver->Create(chunkFilename.c_str(), chunkWidth, chunkHeight, bands, GDT_Float32, nullptr);

            if (ChunkDataset == nullptr) {
                std::cerr << "Failed to make a chunk file for " << chunkCount << std::endl;
                continue;
            }

            // Loop through each band in the dataset
            for (int b = 1; b <= bands; ++b) {
                // buffer for chunk data
                float* buffer = new float[chunkWidth * chunkHeight];

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

void MEMPA::BUF_DEM::dem_mask()
{
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
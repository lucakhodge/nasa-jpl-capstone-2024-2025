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

    XSize = dem_dataset->GetRasterXSize();
    YSize = dem_dataset->GetRasterYSize();
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
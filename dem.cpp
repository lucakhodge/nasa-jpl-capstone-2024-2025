#include "dem.h"

MARS_PROJECT::DEM::DEM(std::string_view ifp, std::string_view ofp) : dem_fp(ifp), out_fp(ofp), dem_dataset(nullptr)
{
    // Check if the filepath input is an empty string.
    if (ofp.empty())
    {
        throw std::invalid_argument("Output file path is empty.");
    }

    // Check that the filepath input exists.
    if (!std::filesystem::exists(out_fp))
    {
        throw std::filesystem::filesystem_error("Output path does not exist: " + std::string(ofp), std::make_error_code(std::errc::no_such_file_or_directory));
    }

    // Check that the filepath input is a directory.
    if (!std::filesystem::is_directory(out_fp))
    {
        throw std::filesystem::filesystem_error("Output path is not a directory: " + std::string(ofp), std::make_error_code(std::errc::no_such_file_or_directory));
    }
}

MARS_PROJECT::DEM::~DEM()
{
    if (dem_dataset)
    {
        GDALClose(dem_dataset);
        dem_dataset = nullptr;
    }
}

GDALDataset *MARS_PROJECT::DEM::dem_load()
{
    dem_dataset = static_cast<GDALDataset *>(GDALOpen(dem_fp.c_str(), GA_ReadOnly));
    if (!dem_dataset)
    {
        throw std::runtime_error("Failed to open input dataset: " + std::string(dem_fp));
    }
    return dem_dataset;
}
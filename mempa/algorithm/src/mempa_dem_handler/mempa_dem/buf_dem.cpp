/* C++ Standard Libraries */
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>

/* GDAL Libraries */
#include <ogrsf_frmts.h>
#include <gdalwarper.h>

/* GeoTIFF Libraries */
#include <tiffio.h>

/* Capstone Project Libraries */
#include "buf_dem.h"

namespace MEMPA {

namespace {
    // Helper function to format error messages
    std::string formatError(const std::string& message, const std::filesystem::path& path) {
        std::stringstream ss;
        ss << "[Error]: " << message << ": " << path;
        return ss.str();
    }
}

BUFFDEM::BUFFDEM(const std::string_view input_filepath, const std::string_view output_filepath) 
    : dem_fp(input_filepath), output_directory(output_filepath),
      source_dataset(nullptr), destination_dataset(nullptr), clip_geometry(nullptr)
{
    std::cout << "Initializing BUFFDEM with file path: " << input_filepath 
              << " and output path: " << output_filepath << std::endl;

    // Validate output directory
    if (output_filepath.empty() || !std::filesystem::exists(output_directory)) {
        throw std::filesystem::filesystem_error(
            formatError("Output directory not found", output_directory),
            std::make_error_code(std::errc::no_such_file_or_directory));
    }

    if (!std::filesystem::is_directory(output_directory)) {
        throw std::filesystem::filesystem_error(
            formatError("Output path is not a directory", output_directory),
            std::make_error_code(std::errc::not_a_directory));
    }

    // Validate input file
    if (input_filepath.empty() || !std::filesystem::exists(dem_fp)) {
        throw std::filesystem::filesystem_error(
            formatError("Input file not found", dem_fp),
            std::make_error_code(std::errc::no_such_file_or_directory));
    }

    if (std::filesystem::is_directory(dem_fp)) {
        throw std::filesystem::filesystem_error(
            formatError("Input path is a directory", dem_fp),
            std::make_error_code(std::errc::is_a_directory));
    }

    // Initialize GDAL
    GDALAllRegister();
    source_dataset = static_cast<GDALDataset*>(GDALOpen(dem_fp.c_str(), GA_ReadOnly));
    if (!source_dataset) {
        throw std::filesystem::filesystem_error(
            formatError("Failed to open input file with GDAL", dem_fp),
            std::make_error_code(std::errc::io_error));
    }

    std::cout << "GDAL dataset opened successfully." << std::endl;
}

BUFFDEM::~BUFFDEM() {
    if (source_dataset) {
        GDALClose(source_dataset);
        source_dataset = nullptr;
    }

    if (destination_dataset) {
        GDALClose(destination_dataset);
        destination_dataset = nullptr;
    }

    if (clip_geometry) {
        OGRGeometryFactory::destroyGeometry(clip_geometry);
        clip_geometry = nullptr;
    }
}

std::vector<std::vector<double>> BUFFDEM::demVector(
    const std::filesystem::path& tif_filepath,
    uint32_t startRow, uint32_t endRow,
    uint32_t startCol, uint32_t endCol)
{
    std::cout << "Generating DEM vector from file path: " << tif_filepath << std::endl;

    // Open TIFF file with smart pointer for automatic cleanup
    struct TIFFDeleter {
        void operator()(TIFF* p) { if (p) TIFFClose(p); }
    };
    std::unique_ptr<TIFF, TIFFDeleter> geotiff(TIFFOpen(tif_filepath.c_str(), "r"));
    
    if (!geotiff) {
        throw std::runtime_error(formatError("Failed to open TIFF file", tif_filepath));
    }

    // Get image dimensions
    uint32_t width, length;
    if (!TIFFGetField(geotiff.get(), TIFFTAG_IMAGEWIDTH, &width) ||
        !TIFFGetField(geotiff.get(), TIFFTAG_IMAGELENGTH, &length)) {
        throw std::runtime_error("Failed to read TIFF dimensions");
    }

    std::cout << "TIFF file dimensions: width = " << width << ", length = " << length 
              << std::endl;

    // Validate ROI boundaries
    if (startRow >= length || endRow >= length || startCol >= width || endCol >= width ||
        startRow > endRow || startCol > endCol) {
        throw std::out_of_range("Specified ROI is out of range or invalid");
    }

    // Calculate ROI dimensions
    uint32_t roi_width = endCol - startCol + 1;
    uint32_t roi_height = endRow - startRow + 1;

    // Allocate buffer with smart pointer
    std::vector<std::vector<double>> tif_map;
    tif_map.reserve(roi_height);

    auto buffer = std::make_unique<uint32_t[]>(width);
    if (!buffer) {
        throw std::bad_alloc();
    }

    // Read scanlines
    for (uint32_t row = startRow; row <= endRow; row++) {
        if (TIFFReadScanline(geotiff.get(), buffer.get(), row) == -1) {
            std::cerr << "Error reading row " << row << " from TIFF file." << std::endl;
            continue;
        }

        std::vector<double> line_data;
        line_data.reserve(roi_width);
        
        for (uint32_t col = startCol; col <= endCol; col++) {
            // Convert elevation data to double and validate
            double elevation = static_cast<double>(buffer[col]);
            if (std::isnan(elevation) || std::isinf(elevation)) {
                std::cerr << "Warning: Invalid elevation at row " << row 
                         << ", col " << col << std::endl;
                elevation = 0.0;  // Use default value for invalid data
            }
            line_data.push_back(elevation);
        }
        
        tif_map.push_back(std::move(line_data));
    }

    // Validate output
    if (tif_map.empty() || tif_map[0].empty()) {
        throw std::runtime_error("Failed to generate valid DEM data");
    }

    std::cout << "DEM vector generated successfully: " 
              << tif_map.size() << "x" << tif_map[0].size() << std::endl;
    return tif_map;
}

} // namespace MEMPA
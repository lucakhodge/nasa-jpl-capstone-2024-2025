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
    }
    if (destination_dataset) {
        GDALClose(destination_dataset);
    }
    if (clip_geometry) {
        OGRGeometryFactory::destroyGeometry(clip_geometry);
    }
}

OGRGeometry* BUFFDEM::demAreaSquare(
    const std::pair<double, double>& center, 
    double sideLength) 
{
    if (sideLength < 0.0) {
        throw std::invalid_argument("[Error]: Side length cannot be negative");
    }

    if (clip_geometry) {
        OGRGeometryFactory::destroyGeometry(clip_geometry);
        clip_geometry = nullptr;
    }

    // Calculate square corners
    double halfSide = sideLength / 2.0;
    double x = center.first;
    double y = center.second;

    OGRPolygon* square = new OGRPolygon();
    OGRLinearRing ring;

    // Add corners in clockwise order
    ring.addPoint(x - halfSide, y - halfSide); // Bottom left
    ring.addPoint(x + halfSide, y - halfSide); // Bottom right
    ring.addPoint(x + halfSide, y + halfSide); // Top right
    ring.addPoint(x - halfSide, y + halfSide); // Top left
    ring.closeRings();

    square->addRing(&ring);
    clip_geometry = square;

    if (!clip_geometry) {
        throw std::runtime_error("[Error]: Failed to create square geometry");
    }

    return clip_geometry;
}

std::vector<std::vector<double>> BUFFDEM::demVector(
    const std::filesystem::path& tif_filepath,
    uint32_t startRow, uint32_t endRow,
    uint32_t startCol, uint32_t endCol)
{
    std::cout << "Generating DEM vector from file path: " << tif_filepath << std::endl;

    struct TIFFDeleter {
        void operator()(TIFF* p) { if (p) TIFFClose(p); }
    };
    std::unique_ptr<TIFF, TIFFDeleter> geotiff(TIFFOpen(tif_filepath.c_str(), "r"));
    
    if (!geotiff) {
        throw std::runtime_error(formatError("Failed to open TIFF file", tif_filepath));
    }

    uint32_t width, length;
    if (!TIFFGetField(geotiff.get(), TIFFTAG_IMAGEWIDTH, &width) ||
        !TIFFGetField(geotiff.get(), TIFFTAG_IMAGELENGTH, &length)) {
        throw std::runtime_error("Failed to read TIFF dimensions");
    }

    std::cout << "TIFF file dimensions: width = " << width << ", length = " << length 
              << std::endl;

    if (startRow >= length || endRow >= length || startCol >= width || endCol >= width ||
        startRow > endRow || startCol > endCol) {
        throw std::out_of_range("Specified ROI is out of range or invalid");
    }

    uint32_t roi_width = endCol - startCol + 1;
    uint32_t roi_height = endRow - startRow + 1;

    std::vector<std::vector<double>> tif_map;
    tif_map.reserve(roi_height);

    auto buffer = std::make_unique<uint32_t[]>(width);
    if (!buffer) {
        throw std::bad_alloc();
    }

    for (uint32_t row = startRow; row <= endRow; row++) {
        if (TIFFReadScanline(geotiff.get(), buffer.get(), row) == -1) {
            std::cerr << "Error reading row " << row << " from TIFF file." << std::endl;
            continue;
        }

        std::vector<double> line_data;
        line_data.reserve(roi_width);
        
        for (uint32_t col = startCol; col <= endCol; col++) {
            double elevation = static_cast<double>(buffer[col]);
            if (std::isnan(elevation) || std::isinf(elevation)) {
                std::cerr << "Warning: Invalid elevation at row " << row 
                         << ", col " << col << std::endl;
                elevation = 0.0;
            }
            line_data.push_back(elevation);
        }
        
        tif_map.push_back(std::move(line_data));
    }

    if (tif_map.empty() || tif_map[0].empty()) {
        throw std::runtime_error("Failed to generate valid DEM data");
    }

    std::cout << "DEM vector generated successfully: " 
              << tif_map.size() << "x" << tif_map[0].size() << std::endl;
    return tif_map;
}

GDALDataset* BUFFDEM::demGet() {
    return source_dataset;
}

OGRGeometry* BUFFDEM::demArea(
    const std::vector<std::pair<double, double>>& coordinates,
    double radius,
    double eccentricity)
{
    if (clip_geometry) {
        OGRGeometryFactory::destroyGeometry(clip_geometry);
        clip_geometry = nullptr;
    }

    switch (coordinates.size()) {
        case 1: {
            if (radius < 0.0) {
                throw std::invalid_argument("[Error]: Radius cannot be negative");
            }
            OGRPoint center(coordinates[0].first, coordinates[0].second);
            clip_geometry = center.Buffer(radius);
            break;
        }
        case 2: {
            if (eccentricity < 0.0 || eccentricity >= 1.0) {
                throw std::invalid_argument("[Error]: Eccentricity must be in range [0, 1)");
            }
            auto& focus1 = coordinates[0];
            auto& focus2 = coordinates[1];
            double foci_distance = std::sqrt(
                std::pow(focus2.first - focus1.first, 2.0) + 
                std::pow(focus2.second - focus1.second, 2.0));
            double semimajor_axis = foci_distance / (2.0 * std::sqrt(1.0 - std::pow(eccentricity, 2.0)));
            double semiminor_axis = semimajor_axis * std::sqrt(1.0 - std::pow(eccentricity, 2.0));

            OGRPolygon* ellipse = new OGRPolygon();
            OGRLinearRing ring;
            const int num_points = 360;
            
            for (int i = 0; i < num_points; i++) {
                double theta = 2.0 * M_PI * static_cast<double>(i) / num_points;
                double x = focus1.first + semimajor_axis * std::cos(theta);
                double y = focus1.second + semiminor_axis * std::sin(theta);
                ring.addPoint(x, y);
            }
            
            ring.closeRings();
            ellipse->addRing(&ring);
            clip_geometry = ellipse;
            break;
        }
        default:
            throw std::invalid_argument("[Error]: Unsupported number of coordinates");
    }

    if (!clip_geometry) {
        throw std::runtime_error("[Error]: Failed to create geometry");
    }

    return clip_geometry;
}

OGRGeometry* BUFFDEM::demAreaGet() {
    return clip_geometry;
}

void BUFFDEM::makeSHP(const std::string& shapefile_name, bool overwrite) {
    static int filecounter = 1;
    shp_fp = output_directory / (shapefile_name + std::to_string(filecounter) + ".shp");

    if (std::filesystem::exists(shp_fp) && !overwrite) {
        throw std::filesystem::filesystem_error(
            "[Error]: Shapefile already exists",
            std::make_error_code(std::errc::file_exists));
    }

    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    if (std::filesystem::exists(shp_fp)) {
        std::filesystem::remove(shp_fp);
    }

    GDALDataset* shapefile = driver->Create(shp_fp.c_str(), 0, 0, 0, GDT_Unknown, nullptr);
    OGRSpatialReference demSRS;
    demSRS.importFromWkt(source_dataset->GetProjectionRef());

    OGRLayer* layer = shapefile->CreateLayer("clip_geometry", &demSRS, wkbUnknown, nullptr);
    OGRFeature* feature = OGRFeature::CreateFeature(layer->GetLayerDefn());
    
    feature->SetGeometry(clip_geometry);
    feature->GetGeometryRef()->assignSpatialReference(&demSRS);
    
    if (layer->CreateFeature(feature) != OGRERR_NONE) {
        OGRFeature::DestroyFeature(feature);
        GDALClose(shapefile);
        throw std::runtime_error("[Error]: Failed to write to shapefile");
    }

    filecounter++;
    OGRFeature::DestroyFeature(feature);
    GDALClose(shapefile);
}

GDALDataset* BUFFDEM::demClip(const std::string& output_name, bool overwrite) {
    static int filecounter = 1;
    output_raster = output_directory / (output_name + std::to_string(filecounter) + ".tif");

    if (std::filesystem::exists(output_raster) && !overwrite) {
        throw std::filesystem::filesystem_error(
            "[Error]: Clipped file already exists",
            std::make_error_code(std::errc::file_exists));
    }

    if (std::filesystem::exists(output_raster)) {
        std::filesystem::remove(output_raster);
    }

    std::string command = "gdalwarp -cutline \"" + shp_fp.string() + 
                         "\" -crop_to_cutline -dstnodata -9999 \"" + 
                         dem_fp.string() + "\" \"" + output_raster.string() + "\"";

    if (std::system(command.c_str()) != 0) {
        throw std::runtime_error("[Error]: gdalwarp command failed");
    }

    destination_dataset = static_cast<GDALDataset*>(GDALOpen(output_raster.c_str(), GA_ReadOnly));
    if (!destination_dataset) {
        throw std::filesystem::filesystem_error(
            "[Error]: Failed to open clipped raster with GDAL",
            std::make_error_code(std::errc::io_error));
    }

    filecounter++;
    return destination_dataset;
}

std::filesystem::path* BUFFDEM::getOutput() {
    return &output_raster;
}

std::vector<std::vector<double>> BUFFDEM::makeRequest(
    const std::vector<std::pair<double, double>>& coordinates,
    double radius_eccentricity,
    const std::string_view input_filepath,
    const std::string_view output_filepath,
    const std::string& output_filename,
    bool useSquareClipping)
{
    try {
        MEMPA::BUFFDEM mars_dem(input_filepath, output_filepath);
        
        if (useSquareClipping) {
            if (coordinates.size() != 1) {
                throw std::invalid_argument("[Error]: Square clipping requires exactly one coordinate pair");
            }
            mars_dem.demAreaSquare(coordinates[0], radius_eccentricity * 2.0);
        } else {
            mars_dem.demArea(coordinates, radius_eccentricity, radius_eccentricity);
        }

        mars_dem.makeSHP(output_filename + "_shape", true);
        mars_dem.demClip(output_filename + "_clip", true);
        GDALDataset* dataset = mars_dem.demGet();
        int xSize = dataset->GetRasterXSize();
        int ySize = dataset->GetRasterYSize();
        return mars_dem.demVector(*mars_dem.getOutput(), 0, ySize - 1, 0, xSize - 1);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return std::vector<std::vector<double>>();
    }
}

} // namespace MEMPA
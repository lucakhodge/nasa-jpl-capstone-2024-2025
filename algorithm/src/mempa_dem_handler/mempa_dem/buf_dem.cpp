/* C++ Standard Libraries */
#include <iostream>
#include <cmath>

/* GDAL Libraries
   Check dependencies in README */
#include <ogrsf_frmts.h>
#include <gdalwarper.h>

/* GeoTIFF Libraries */
#include <tiffio.h>

/* Capstone Project Libraries */
#include "buf_dem.h"

/**
 * @brief Constructs a new @c MEMPA::BUFFDEM object.
 *
 * @details
 * Initializes the DEM object by setting input and output file paths and verifying their validity.
 * Also registers GDAL to ensure raster files can be opened and processed.
 *
 * @param input_filepath Path to the input DEM GeoTIFF file.
 * @param output_filepath Directory path where generated files (e.g., shapefiles, clipped rasters) will be saved.
 *
 * @throws std::filesystem::filesystem_error if:
 * - The input file does not exist or is not a file.
 * - The output directory does not exist or is not a directory.
 * - GDAL fails to open the input DEM file.
 *
 * @note Assumes that the provided DEM file uses a compatible coordinate system.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-12-8
 */
MEMPA::BUFFDEM::BUFFDEM(const std::string_view input_filepath, const std::string_view output_filepath) : dem_fp(input_filepath), output_directory(output_filepath)
{
    /*
    Output file path error checking:
    - If the output file path is empty.
    - If the output file path does not exist.
    - If the output file path is not a directory.
    */

    if (output_filepath.empty() || !std::filesystem::exists(output_directory))
    {
        throw std::filesystem::filesystem_error("[Error]: Output directory not found", std::make_error_code(std::errc::no_such_file_or_directory));
    }

    if (!std::filesystem::is_directory(output_directory))
    {
        throw std::filesystem::filesystem_error("[Error]: Output path is not a directory", std::make_error_code(std::errc::not_a_directory));
    }

    /*
    Input file path error checking:
    - If the input file path is empty.
    - If the input file path does not exist.
    - If the input file path is a directory.
    */

    if (input_filepath.empty() || !std::filesystem::exists(dem_fp))
    {
        throw std::filesystem::filesystem_error("[Error]: Input file not found", std::make_error_code(std::errc::no_such_file_or_directory));
    }

    if (std::filesystem::is_directory(dem_fp))
    {
        throw std::filesystem::filesystem_error("[Error]: Input path is a directory", std::make_error_code(std::errc::is_a_directory));
    }

    GDALAllRegister();
    source_dataset = static_cast<GDALDataset *>(GDALOpen(dem_fp.c_str(), GA_ReadOnly));
    if (!source_dataset)
    {
        throw std::filesystem::filesystem_error("[Error]: Failed to open input file with GDAL", std::make_error_code(std::errc::io_error));
    }

    clip_geometry = nullptr;
    destination_dataset = nullptr;
}

/**
 * @brief Destroys the @c MEMPA::BUFFDEM object and cleans up allocated resources.
 *
 * @details
 * Ensures that GDAL datasets and OGR geometry objects are properly closed and deallocated.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-12-8
 */
MEMPA::BUFFDEM::~BUFFDEM()
{
    if (source_dataset)
    {
        GDALClose(source_dataset);
        source_dataset = nullptr;
    }

    if (destination_dataset)
    {
        GDALClose(destination_dataset);
        destination_dataset = nullptr;
    }

    if (clip_geometry)
    {
        OGRGeometryFactory::destroyGeometry(clip_geometry);
        clip_geometry = nullptr;
    }
}

/**
 * @brief Retrieves the loaded DEM dataset.
 *
 * @return GDALDataset* Pointer to the loaded DEM dataset, or @c nullptr if no dataset is loaded.
 *
 * @note The returned pointer should not be deallocated externally.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-12-8
 */
GDALDataset *MEMPA::BUFFDEM::demGet()
{
    return source_dataset;
}

/**
 * @brief Defines a geometric clipping area (circle or ellipse) based on input coordinates.
 *
 * @details
 * Depending on the number of coordinates provided:
 * - A single coordinate with a radius generates a circular clipping area.
 * - Two coordinates with an eccentricity generate an elliptical area.
 *
 * @param coordinates Vector of (latitude, longitude) pairs:
 *   - Length 1: Defines the center of a circular area.
 *   - Length 2: Defines two foci of an ellipse.
 * @param radius Radius for a circular area. Used only for a single coordinate.
 * @param eccentricity Eccentricity for an elliptical area. Must be in the range [0, 1). Used only for two coordinates.
 *
 * @return OGRGeometry* Pointer to the resulting geometry (circle or ellipse).
 *
 * @throws std::invalid_argument If:
 * - Radius is negative.
 * - Eccentricity is out of range [0, 1).
 * - An unsupported number of coordinates is provided.
 * @throws std::runtime_error If geometry creation fails.
 *
 * @note Input coordinates must use the Mars 2000 projection system, with valid ranges:
 *   - Latitude: [-90.0, 90.0]
 *   - Longitude: [-180.0, 180.0]
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-12-8
 */
OGRGeometry *MEMPA::BUFFDEM::demArea(const std::vector<std::pair<double, double>> coordinates, const double radius, const double eccentricity)
{
    if (clip_geometry)
    {
        OGRGeometryFactory::destroyGeometry(clip_geometry);
        clip_geometry = nullptr;
    }
    switch (coordinates.size())
    {
    case 1:
    {
        if (radius < 0.0)
        {
            throw std::invalid_argument("[Error]: Radius cannot be negative");
        }
        std::pair<double, double> center = coordinates[0];
        OGRPoint center_point(center.first, center.second);
        clip_geometry = center_point.Buffer(radius);
        break;
    }
    case 2:
    {
        if (eccentricity < 0.0 || eccentricity >= 1.0)
        {
            throw std::invalid_argument("[Error]: Eccentricity must be in range [0, 1)");
        }
        std::pair<double, double> focus1 = coordinates[0];
        std::pair<double, double> focus2 = coordinates[1];
        double foci_distance = std::sqrt(std::pow(focus2.first - focus1.first, 2.0) + std::pow(focus2.second - focus1.second, 2.0));
        double semimajor_axis = foci_distance / (2.0 * std::sqrt(1.0 - std::pow(eccentricity, 2.0)));
        double semiminor_axis = semimajor_axis * std::sqrt(1.0 - std::pow(eccentricity, 2.0));
        OGRPolygon *ellipse = new OGRPolygon();
        OGRLinearRing ring;
        const int num_points = 360;
        for (int i = 0; i < num_points; i++)
        {
            double theta = 2.0 * M_PI * (double)i / (double)num_points;
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
    {
        throw std::invalid_argument("[Error]: Unsupported number of coordinates");
        break;
    }
    }
    if (clip_geometry == nullptr)
    {
        throw std::runtime_error("[Error]: Failed to create geometry");
    }
    return clip_geometry;
}

/**
 * @brief Retrieves the current clipping geometry.
 *
 * @return OGRGeometry* Pointer to the existing geometry, or @c nullptr if no geometry is defined.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-12-8
 */
OGRGeometry *MEMPA::BUFFDEM::demAreaGet()
{
    return clip_geometry;
}

/**
 * @brief Creates a shapefile representing the current clipping geometry.
 *
 * @details
 * Saves the defined clipping geometry (e.g., circle or ellipse) to a shapefile for visualization or further processing.
 *
 * @param shapefile_name Base name for the output shapefile (without extension).
 * The function appends a numeric suffix and ".shp" extension.
 * @param overwrite Whether to overwrite an existing shapefile with the same name:
 *   - @c true: Overwrites the existing file.
 *   - @c false: Throws an exception if a file with the same name exists.
 *
 * @throws std::filesystem::filesystem_error If file creation fails.
 * @throws std::runtime_error If GDAL fails to create the shapefile or write its contents.
 *
 * @note The output shapefile is saved in the specified output directory.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-12-8
 */
void MEMPA::BUFFDEM::makeSHP(const std::string &shapefile_name, const bool overwrite)
{
    static int filecounter = 1;
    shp_fp = output_directory / (shapefile_name + std::to_string(filecounter) + ".shp");

    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");

    if (std::filesystem::exists(shp_fp))
    {
        if (!overwrite)
        {
            throw std::filesystem::filesystem_error("[Error]: Shapefile already exists", std::make_error_code(std::errc::file_exists));
        }
        std::filesystem::remove(shp_fp);
    }

    GDALDataset *shapefile = driver->Create(shp_fp.c_str(), 0, 0, 0, GDT_Unknown, nullptr);

    OGRSpatialReference demSRS;
    demSRS.importFromWkt(source_dataset->GetProjectionRef());

    OGRLayer *layer = shapefile->CreateLayer("clip_geometry", &demSRS, wkbUnknown, nullptr);

    OGRFeature *feature = OGRFeature::CreateFeature(layer->GetLayerDefn());
    feature->SetGeometry(clip_geometry);
    feature->GetGeometryRef()->assignSpatialReference(&demSRS);
    OGRErr err = layer->CreateFeature(feature);
    if (err != OGRERR_NONE)
    {
        throw std::runtime_error("[Error]: Failed to write to shapefile");
    }

    filecounter++;
    OGRFeature::DestroyFeature(feature);
    GDALClose(shapefile);
}

/**
 * @brief Clips the DEM dataset to the current clipping geometry and generates a smaller raster file.
 *
 * @details
 * Uses GDAL's @c gdalwarp tool to perform the clipping operation based on the shapefile created with @ref makeSHP().
 *
 * @param output_name Base name for the output clipped raster file (without extension).
 * The function appends a numeric suffix and ".tif" extension.
 * @param overwrite Whether to overwrite an existing raster file with the same name:
 *   - @c true: Overwrites the existing file.
 *   - @c false: Throws an exception if a file with the same name exists.
 *
 * @return GDALDataset* Pointer to the clipped raster dataset.
 *
 * @throws std::filesystem::filesystem_error If file creation fails.
 * @throws std::runtime_error If GDAL or @c gdalwarp fails to execute.
 *
 * @note The output raster file is saved in the specified output directory.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-12-8
 */
GDALDataset *MEMPA::BUFFDEM::demClip(const std::string &output_name, const bool overwrite)
{
    static int filecounter = 1;
    output_raster = output_directory / (output_name + std::to_string(filecounter) + ".tif");

    if (std::filesystem::exists(output_raster))
    {
        if (!overwrite)
        {
            throw std::filesystem::filesystem_error("[Error]: Clipped file already exists", std::make_error_code(std::errc::file_exists));
        }
        std::filesystem::remove(output_raster);
    }

    std::string command = "(gdalwarp -cutline \"" + shp_fp.string() + "\" -crop_to_cutline -dstnodata -9999 \"" + dem_fp.string() + "\" \"" + output_raster.string() + "\")";

    int result = std::system(command.c_str());
    if (result != 0)
    {
        throw std::runtime_error("[Error]: gdalwarp command failed");
    }

    destination_dataset = static_cast<GDALDataset *>(GDALOpen(output_raster.c_str(), GA_ReadOnly));
    if (!destination_dataset)
    {
        throw std::filesystem::filesystem_error("[Error]: Failed to open clipped raster with GDAL", std::make_error_code(std::errc::io_error));
    }

    filecounter++;
    return destination_dataset;
}

/**
 * @brief Creates a 2D vector map from an input raster (.tif) file.
 *
 * @details Should return the vector to be directly passed to the pathfinding algorithm.
 *          Currently untested.
 *
 * @todo Test function incorporation into rover algorithm input.
 *
 * @param tif_filepath Filepath for a GeoTIFF raster to be read.
 *
 * @throws std::bad_alloc If memory allocation fails.
 *
 * @return std::vector<std::vector<double>> A 2D vector map of the input raster.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2025-2-3
 */
std::vector<std::vector<double>> MEMPA::BUFFDEM::demVector(const std::filesystem::path &tif_filepath)
{
    std::vector<std::vector<double>> tif_map;

    TIFF *geotiff = TIFFOpen(tif_filepath.c_str(), "r");

    uint32_t width, length;
    TIFFGetField(geotiff, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(geotiff, TIFFTAG_IMAGELENGTH, &length);

    uint32_t *buffer = (uint32_t *)_TIFFmalloc(sizeof(uint32_t) * width);
    if (buffer == nullptr)
    {
        TIFFClose(geotiff);
        throw std::bad_alloc();
    }

    for (uint32_t row = 0; row < width; row++)
    {
        TIFFReadScanline(geotiff, buffer, row);
        std::vector<double> line_data;
        for (uint32_t col = 0; col < length; col++)
        {
            line_data.push_back(static_cast<double>(buffer[col]));
        }
        tif_map.push_back(line_data);
    }

    _TIFFfree(buffer);
    TIFFClose(geotiff);
    return tif_map;
}

/**
 * @brief Returns a pointer to the output path of the clipped raster.
 *
 * @return std::filesystem::path* The path to the clipped raster created by the @c demClip function.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2025-2-3
 */
std::filesystem::path *MEMPA::BUFFDEM::getOutput()
{
    return &output_raster;
}

/**
 * @brief Single use function for running the entirety of the DEM handler in one call. Simulator should be calling this each time.
 *
 * @param coordinates Vector of coordinate pairs. For circles, should contain one pair of coordinates.
 * @param radius_eccentricity Value for eccentricity of radius. Should be using the radius for circles.
 * @param input_filepath The filepath for the .tif file to be accessed.
 * @param output_filepath The directory location to save new files to.
 * @param output_filename The general name for new files to be identified with. Do not include any file extentions or directory characters/
 * @return std::vector<std::vector<double>> A 2D vector of the values in the DEM.
 * 
 * 
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2025-2-17
 */
std::vector<std::vector<double>> MEMPA::BUFFDEM::makeRequest(const std::vector<std::pair<double, double>> coordinates, const double radius_eccentricity, const std::string_view input_filepath, const std::string_view output_filepath, const std::string output_filename)
{
    try
    {
        MEMPA::BUFFDEM mars_dem(input_filepath, output_filepath);
        mars_dem.demArea(coordinates, radius_eccentricity, radius_eccentricity);
        mars_dem.makeSHP(output_filename + " shape", true);
        mars_dem.demClip(output_filename + " clip", true);
        std::vector<std::vector<double>> clip_vector = mars_dem.demVector(*mars_dem.getOutput());
        return clip_vector;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return std::vector<std::vector<double>>();
    }
}
/* C++ Standard Libraries */
#include <cmath>

/* GDAL libraries. Check dependencies in README. */
#include <ogrsf_frmts.h>
#include <gdalwarper.h>

/* Capstone Project Libraries */
#include "buf_dem.h"

/**
 * @brief Constructs a new @c MEMPA::DEM object.
 *
 * @details
 * This constructor initializes a @ref BUF_DEM object by setting the input GeoTIFF file path and the output directory path.
 * It performs validation on the output directory to ensure that it exists and is a valid directory.
 *
 * @param input_filepath The file path to the input DEM GeoTIFF image. This file contains the Digital Elevation Model data
 *            to be processed.
 * @param output_filepath The output directory where new files, generated from the DEM data (such as chunked DEM data),
 *            will be saved. If the directory does not exist or is invalid, an exception is thrown.
 *
 * @throws std::filesystem::filesystem_error Thrown if the the input file or output directory are inaccessible.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-10-01
 */
MEMPA::BUFFDEM::BUFFDEM(const std::string_view input_filepath, const std::string_view output_filepath) : dem_fp(input_filepath), out_fp(output_filepath)
{
    /*
    Output file path error checking:
    - If the output file path is empty.
    - If the output file path does not exist.
    - If the output file path is not a directory.
    */

    if (output_filepath.empty() || !std::filesystem::exists(out_fp))
    {
        throw std::filesystem::filesystem_error(std::string(output_filepath), std::make_error_code(std::errc::no_such_file_or_directory));
    }

    if (!std::filesystem::is_directory(out_fp))
    {
        throw std::filesystem::filesystem_error(std::string(output_filepath), std::make_error_code(std::errc::not_a_directory));
    }

    /*
    Input file path error checking:
    - If the input file path is empty.
    - If the input file path does not exist.
    - If the input file path is a directory.
    */

    if (input_filepath.empty() || !std::filesystem::exists(dem_fp))
    {
        throw std::filesystem::filesystem_error(std::string(input_filepath), std::make_error_code(std::errc::no_such_file_or_directory));
    }

    if (std::filesystem::is_directory(dem_fp))
    {
        throw std::filesystem::filesystem_error(std::string(input_filepath), std::make_error_code(std::errc::is_a_directory));
    }

    GDALAllRegister();
    dem_dataset = static_cast<GDALDataset *>(GDALOpen(dem_fp.c_str(), GA_ReadOnly));
    if (!dem_dataset)
    {
        const char *GDAL_errMsg = CPLGetLastErrorMsg();
        throw std::filesystem::filesystem_error((GDAL_errMsg ? GDAL_errMsg : "unknown error"), std::make_error_code(std::errc::io_error));
    }

    clip_area = nullptr;
    clip_dataset = nullptr;
    warp_options = nullptr;
}

/**
 * @brief Destroys the @c MEMPA::DEM object.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-10-01
 */
MEMPA::BUFFDEM::~BUFFDEM()
{
    if (dem_dataset)
    {
        GDALClose(dem_dataset);
        dem_dataset = nullptr;
    }

    if (clip_dataset)
    {
        GDALClose(clip_dataset);
        clip_dataset = nullptr;
    }

    if (clip_area)
    {
        OGRGeometryFactory::destroyGeometry(clip_area);
        clip_area = nullptr;
    }

    if (warp_options)
    {
        if (warp_options->pTransformerArg)
        {
            GDALDestroyGenImgProjTransformer(warp_options->pTransformerArg);
            warp_options->pTransformerArg = nullptr;
        }
        if (warp_options->panSrcBands)
        {
            CPLFree(warp_options->panSrcBands);
            warp_options->panSrcBands = nullptr;
        }
        if (warp_options->panDstBands)
        {
            CPLFree(warp_options->panDstBands);
            warp_options->panDstBands = nullptr;
        }
        GDALDestroyWarpOptions(warp_options);
        warp_options = nullptr;
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
GDALDataset *MEMPA::BUFFDEM::demGet()
{
    return dem_dataset;
}

/**
 * @brief Defines an area to be clipped out of the current raster. Input coordinates must be a vector of double pairs.
 *
 *
 * @return OGRGeometry* pointer for the shape of the study area for clipping.
 *
 *
 * @note Input coordinates must be using the Mars 2000 projection!
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-12-1
 */
OGRGeometry *MEMPA::BUFFDEM::demArea(const std::vector<std::pair<double, double>> coordinates, const double radius, const double eccentricity)
{
    if (clip_area)
    {
        OGRGeometryFactory::destroyGeometry(clip_area);
        clip_area = nullptr;
    }
    switch (coordinates.size())
    {
    case 1:
    {
        if (radius < 0.0)
        {
            throw std::invalid_argument("the radius (" + std::to_string(radius) + ") cannot be negative");
        }
        std::pair<double, double> center = coordinates[0];
        OGRPoint center_point(center.first, center.second);
        clip_area = center_point.Buffer(radius);
        if (clip_area == nullptr)
        {
            throw std::runtime_error("failed to create circle of radius " + std::to_string(radius) + " for point (" + std::to_string(center.first) + ", " + std::to_string(center.second) + ")");
        }
        break;
    }
    case 2:
    {
        if (eccentricity < 0.0 || eccentricity >= 1.0)
        {
            throw std::invalid_argument("the eccentricity (" + std::to_string(eccentricity) + ") must be in the range [0, 1)");
        }
        std::pair<double, double> focus1 = coordinates[0];
        std::pair<double, double> focus2 = coordinates[1];
        double foci_distance = std::sqrt(std::pow(focus2.first - focus1.first, 2.0) + std::pow(focus2.second - focus1.second, 2.0));
        double semimajor_axis = foci_distance / (2.0 * std::sqrt(1.0 - eccentricity * eccentricity));
        double semiminor_axis = semimajor_axis * std::sqrt(1.0 - eccentricity * eccentricity);
        OGRPolygon *ellipse = new OGRPolygon(); // throws bad_alloc
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
        clip_area = ellipse;
        if (clip_area == nullptr)
        {
            throw std::runtime_error("failed to create ellipse of eccentricity " + std::to_string(eccentricity) + " for points (" + std::to_string(focus1.first) + ", " + std::to_string(focus1.second) + ") and (" + std::to_string(focus2.first) + ", " + std::to_string(focus2.second) + ")");
        }
        break;
    }
    default:
    {
        throw std::invalid_argument("unsupported number of coordinates: " + std::to_string(coordinates.size()));
        break;
    }
    }
    return clip_area;
}

OGRGeometry *MEMPA::BUFFDEM::demAreaGet()
{
    return clip_area;
}

/**
 * @brief Creates a new shapefile for the current study area.
 *
 *
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-12-4
 */
void MEMPA::BUFFDEM::makeSHP(const std::string &shapefile_name, const bool overwrite)
{
    static int filecounter = 1;
    std::filesystem::path shapefile_path = out_fp / (shapefile_name + std::to_string(filecounter) + ".shp");

    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");

    if (std::filesystem::exists(shapefile_path))
    {
        if (overwrite)
        {
            std::filesystem::remove(shapefile_path);
        }
        else
        {
            throw std::filesystem::filesystem_error(std::string(shapefile_path), std::make_error_code(std::errc::file_exists));
        }
    }

    GDALDataset *shapefile = driver->Create(shapefile_path.c_str(), 0, 0, 0, GDT_Unknown, nullptr);

    OGRSpatialReference demSRS;
    demSRS.importFromWkt(dem_dataset->GetProjectionRef());

    OGRLayer *layer = shapefile->CreateLayer("clip_area", &demSRS, wkbUnknown, nullptr);

    OGRFeature *feature = OGRFeature::CreateFeature(layer->GetLayerDefn());
    feature->SetGeometry(clip_area);
    feature->GetGeometryRef()->assignSpatialReference(&demSRS);
    OGRErr err = layer->CreateFeature(feature);
    if (err != OGRERR_NONE)
    {
        throw std::runtime_error("failed to create shapefile: " + err);
    }

    filecounter++;
    OGRFeature::DestroyFeature(feature);
    GDALClose(shapefile);
}

GDALDataset *MEMPA::BUFFDEM::demClip(const std::string &chunkfile_name, const bool overwrite)
{
    static int filecounter = 1;
    std::filesystem::path clipfile_path = out_fp / (chunkfile_name + std::to_string(filecounter) + ".tif");

    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");

    if (std::filesystem::exists(clipfile_path))
    {
        if (overwrite)
        {
            std::filesystem::remove(clipfile_path);
        }
        else
        {
            throw std::filesystem::filesystem_error(std::string(clipfile_path), std::make_error_code(std::errc::file_exists));
        }
    }

    if (clip_dataset)
    {
        GDALClose(clip_dataset);
        clip_dataset = nullptr;
    }

    clip_dataset = (GDALDataset *)driver->CreateCopy(clipfile_path.c_str(), dem_dataset, 0, nullptr, nullptr, nullptr);
    CPLErr err = clip_dataset->GetRasterBand(1)->Fill(0);

    const char *src_srs = dem_dataset->GetProjectionRef();
    const char *dst_srs = clip_dataset->GetProjectionRef();

    warp_options = GDALCreateWarpOptions();
    warp_options->hSrcDS = dem_dataset;
    warp_options->hDstDS = clip_dataset;
    warp_options->nBandCount = 1;
    warp_options->panSrcBands = (int *)CPLMalloc(sizeof(int) * warp_options->nBandCount);
    warp_options->panSrcBands[0] = 1;
    warp_options->panDstBands = (int *)CPLMalloc(sizeof(int) * warp_options->nBandCount);
    warp_options->panDstBands[0] = 1;
    warp_options->pfnProgress = GDALTermProgress;
    warp_options->hCutline = clip_area;

    warp_options->pTransformerArg = GDALCreateGenImgProjTransformer(dem_dataset, src_srs, clip_dataset, dst_srs, true, 1000, 1);
    warp_options->pfnTransformer = GDALGenImgProjTransform;

    GDALWarpOperation oOperation;
    oOperation.Initialize(warp_options);
    oOperation.ChunkAndWarpImage(0, 0, GDALGetRasterXSize(dem_dataset), GDALGetRasterYSize(dem_dataset));

    return clip_dataset;
}

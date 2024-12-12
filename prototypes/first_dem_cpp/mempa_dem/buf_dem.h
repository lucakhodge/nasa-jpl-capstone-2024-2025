#ifndef DEM_H
#define DEM_H

/* C++ Standard Libraries */
#include <filesystem>
#include <string_view>

/* GDAL libraries. Check dependencies in README. */
#include <gdal_priv.h>
#include <gdalwarper.h>
#include <ogrsf_frmts.h>

/**
 * @namespace MEMPA
 * @brief Namespace for the Mars Elevation Model Pathfinding Algorithm (MEMPA).
 *
 * @details
 * This namespace includes classes and functions for loading and processing
 * Digital Elevation Model (DEM) data. It provides algorithms to analyze slope
 * to allow an autonomous Mars rover to find efficient paths for navigation,
 * while avoiding risky terrain that could cause accidents.
 *
 * The MEMPA namespace contains:
 * - @ref BUF_DEM For opening and chunking DEM data.
 *
 * @todo Classes for processing elevation data for pathfinding.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-10-01
 */
namespace MEMPA
{
    /**
     * @class BUF_DEM
     * @brief A class for managing Digital Elevation Model (DEM) data.
     *
     * @details
     * The @ref BUF_DEM class is responsible for loading and preparing DEM data
     * using the GDAL library. It handles a pointer to a GDALDataset
     * and provides functions for accessing and processing elevation data.
     *
     * This class includes:
     * - @c dem_fp A pointer to a GDALDataset that represents the loaded DEM.
     * - @c output_directory A file path for the input DEM file.
     * - @c source_dataset An output directory for saving processed data and results.
     *
     * The class provides functionality to:
     * - Load a DEM file from the specified file path.
     *
     * This class is designed to facilitate easy integration
     * of DEM data into larger geospatial processing workflows
     * for the Mars Buffs CSCI senior project. (Fall 2024 - Spring 2025)
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     * @date 2024-10-01
     */
    class BUFFDEM
    {
    public:
        BUFFDEM(const std::string_view input_filepath, const std::string_view output_filepath);
        ~BUFFDEM();
        GDALDataset *demGet();
        OGRGeometry *demArea(const std::vector<std::pair<double, double>> coordinates, const double radius, const double eccentricity);
        OGRGeometry *demAreaGet();
        void makeSHP(const std::string &shapefile_name, const bool overwrite);
        GDALDataset *demClip(const std::string &chunkfile_name, const bool overwrite);

    private:
        std::filesystem::path dem_fp;           // Original DEM filepath.
        std::filesystem::path output_directory; // Output directory.
        std::filesystem::path shp_fp;           // Shape filepath.
        GDALDataset *source_dataset;            // Pointer to the GDAL dataset.
        GDALDataset *destination_dataset;       // Clipped dataset chunk for the algorithm.
        OGRGeometry *clip_geometry;             // Polygon for defining the clip.
        GDALWarpOptions *psWarpOptions;         // Warp options for clipping.
    };
};

#endif // DEM_H
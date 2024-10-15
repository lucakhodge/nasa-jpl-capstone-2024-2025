#ifndef DEM_H
#define DEM_H

/* C++ Standard Libraries */
#include <string>
#include <filesystem>

/* GDAL libraries. Check dependencies in README. */
#include <gdal_priv.h>

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
     * - @c out_fp A file path for the input DEM file.
     * - @c dem_dataset An output directory for saving processed data and results.
     *
     * The class provides functionality to:
     * - Load a DEM file from the specified file path.
     *
     * @todo Retrieve metadata and information about the DEM.
     * @todo Divide the DEM into smaller, more useful chunks.
     * @todo Save processed outputs to the designated directory.
     *
     * This class is designed to facilitate easy integration
     * of DEM data into larger geospatial processing workflows
     * for the Mars Buffs CSCI senior project. (Fall 2024 - Spring 2025)
     *
     * @author Ryan Wagster <ryan.wagster@colorado.edu>
     * @date 2024-10-01
     */
    class BUF_DEM
    {
    public:
        BUF_DEM(std::string ifp, std::string ofp);
        ~BUF_DEM();

        GDALDataset *dem_load();
        GDALDataset *dem_grab();
        void dem_chunk();
        void dem_mask();
        void dem_safeid();
        void dem_filter();
        void dem_infostream();
        void dem_getdata();

    private:
        std::filesystem::path dem_fp; // Original DEM filepath.
        std::filesystem::path out_fp; // Output directory.
        GDALDataset *dem_dataset;     // Pointer to the GDAL dataset.
    };
};

#endif // DEM_H
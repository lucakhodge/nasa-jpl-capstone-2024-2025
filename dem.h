#ifndef DEM_H
#define DEM_H

/* C++ Standard Libraries */
#include <string>
#include <filesystem>

/* GDAL libraries. Check dependencies in README. */
#include <gdal_priv.h>

namespace MARS_PROJECT
{
    class DEM
    {
    public:
        DEM(std::string_view ifp, std::string_view ofp); // Constructor
        ~DEM();                                          // Destructor

        GDALDataset* dem_load();

    private:
        std::filesystem::path dem_fp; // Original DEM filepath.
        std::filesystem::path out_fp; // Output directory.
        GDALDataset *dem_dataset;     // Pointer to the GDAL dataset.
    };
};

#endif // DEM_H
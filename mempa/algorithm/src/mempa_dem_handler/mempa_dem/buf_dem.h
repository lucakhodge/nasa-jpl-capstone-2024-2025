#ifndef DEM_H
#define DEM_H

#include <filesystem>
#include <string_view>
#include <vector>
#include <utility>
#include <gdal_priv.h>

namespace MEMPA {

class BUFFDEM {
public:
    BUFFDEM(const std::string_view input_filepath, const std::string_view output_filepath);
    ~BUFFDEM();
    
    // Vector generation methods
    std::vector<std::vector<double>> demVector(const std::filesystem::path& tif_filepath, 
        uint32_t startRow, uint32_t endRow, uint32_t startCol, uint32_t endCol);
    
    // Geometry handling methods
    GDALDataset* demGet();
    OGRGeometry* demArea(const std::vector<std::pair<double, double>>& coordinates, 
        double radius, double eccentricity);
    OGRGeometry* demAreaSquare(const std::pair<double, double>& center, double sideLength);
    OGRGeometry* demAreaGet();
    
    // File operations
    void makeSHP(const std::string& shapefile_name, bool overwrite);
    GDALDataset* demClip(const std::string& output_name, bool overwrite);
    std::filesystem::path* getOutput();

    // High-level operations
    std::vector<std::vector<double>> makeRequest(
        const std::vector<std::pair<double, double>>& coordinates,
        double radius_eccentricity,
        const std::string_view input_filepath,
        const std::string_view output_filepath,
        const std::string& output_filename,
        bool useSquareClipping = false);

private:
    std::filesystem::path dem_fp;
    std::filesystem::path output_directory;
    std::filesystem::path shp_fp;
    std::filesystem::path output_raster;
    GDALDataset* source_dataset;
    GDALDataset* destination_dataset;
    OGRGeometry* clip_geometry;
};

} // namespace MEMPA

#endif // DEM_H
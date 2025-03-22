#include "DemHandler.h"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <DEM Filepath> <Chunk Size>\n";
        return 1;
    }

    const char *demFilepath = argv[1];
    const int chunkSize = std::stoi(argv[2]);

    try
    {
        // Create the DemHandler object
        mempa::DemHandler marsRaster(demFilepath);

        // Example coordinates (latitude, longitude)
        std::pair<double, double> userCoordinates = std::make_pair(-150.1011, 40.4233);

        // Convert to image coordinates
        std::pair<int, int> imageCoordinates = marsRaster.transformCoordinates(userCoordinates.first, userCoordinates.second);

        // Read the elevation chunk
        std::vector<std::vector<float>> elevationDataChunk = marsRaster.readSquareChunk(imageCoordinates, chunkSize);

        constexpr int lineWidth = 5;
        for (const auto &row : elevationDataChunk)
        {
            for (float value : row)
            {
                std::cout << std::setw(lineWidth) << value; // Align values in a neat column
            }
            std::cout << "\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

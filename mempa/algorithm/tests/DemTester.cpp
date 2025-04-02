#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include "DemHandler/DemHandler.hpp"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <DEM Filepath> <Chunk Size>\n";
    return 1;
  }

  const char *demFilepath = argv[1];
  const int chunkSize = std::stoi(argv[2]);

  try {
    // Create the DemHandler object
    mempa::DemHandler marsRaster(demFilepath);

    // Example coordinates (latitude, longitude)
    std::pair<double, double> userCoordinates =
        std::make_pair(-146.1011, 40.4233);
    std::pair<double, double> userCoordinates2 =
        std::make_pair(-151.0011, 37.5233);

    // Convert to image coordinates
    std::pair<int, int> imageCoordinates =
        marsRaster.transformCoordinates(userCoordinates);
    std::pair<int, int> imageCoordinates2 =
        marsRaster.transformCoordinates(userCoordinates2);

    std::pair<std::pair<int, int>, std::pair<int, int>> rectCoordinates =
        std::make_pair(imageCoordinates, imageCoordinates2);

    // Read the elevation chunk
    std::vector<std::vector<float>> elevationDataChunk =
        marsRaster.readRectangleChunk(rectCoordinates, chunkSize);

    // Ensure pixel resolution is 200
    const double sizetest = marsRaster.getImageResolution();

    const char *ci_env = std::getenv("CI");
    if (!ci_env) // If CI variable is not set
    {
      constexpr int lineWidth = 5;
      for (auto row : elevationDataChunk) {
        for (float value : row) {
          std::cout << std::setw(lineWidth)
                    << value; // Align values in a neat column
        }
        std::cout << "\n";
      }

      std::cout << "Image resolution: " << sizetest << "m\n\n";
      //           << marsRaster.getMinElevation() << "\n"
      //           << marsRaster.getMaxElevation() << "\n\n";
    }
  } catch (const std::exception &demError) {
    std::cerr << "Error: " << demError.what() << "\n";
    return 1;
  }

  return 0;
}

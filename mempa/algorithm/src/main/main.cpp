/* mempa::CLI */
#include "CLI.hpp"

/* mempa::DemHandler */
#include "../dem-handler/DemHandler.hpp"

/* mempa::RoverSimulator */
#include "../rover-simulator/RoverSimulator.hpp"

/* Dijkstras */
#include "../src/rover-pathfinding-module/dijkstras.hpp"
#include "../src/rover-pathfinding-module/NewDijkstras.hpp"

/* PathLogger */
#include "../logger/PathLogger.hpp"
#include "../metrics/Metrics.hpp"
#include <iostream>

/* nlohmann-json3-dev */
#include <nlohmann/json.hpp>

/* C++ Standard Libraries */
#include <iostream>
#include <stdexcept>

/**
 * @brief Main function to run the MEMPA project.
 *
 * @param argc
 * @param argv
 * @return int
 *
 * @author Brock Hoos <brock.hoos@colorado.edu>
 * @author Ryan Wagster <rywa2447@colorado.edu>
 */
int main(int argc, char *argv[]) {

  std::vector<std::vector<float>> heightmap2 = {  {0,0,45,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}};

  std::vector<std::vector<float>> heightmap3 = {  {0,0,46,0,0}, 
                                      {0,0,46,0,0}, 
                                      {0,0,46,0,0}, 
                                      {0,0,46,0,0}, 
                                      {0,0,46,0,0}};
                                    
  std::vector<std::vector<float>> heightmap4 = {
      {29.88, 58.45, 85.44, 39.52, 13.17, 76.88, 72.87, 35.43, 4.13, 2.28},
      {88.60, 2.53, 61.67, 63.92, 47.15, 87.80, 37.40, 77.93, 66.13, 94.16},
      {12.29, 58.13, 60.67, 75.29, 13.88, 51.75, 98.61, 41.15, 74.55, 73.52},
      {65.25, 83.14, 3.58, 94.01, 33.84, 5.39, 97.50, 95.89, 75.48, 4.45},
      {23.82, 42.12, 84.21, 54.86, 58.99, 11.71, 5.30, 17.88, 80.06, 65.96},
      {41.52, 61.60, 35.88, 82.56, 80.39, 18.18, 8.29, 47.61, 88.11, 84.85},
      {56.58, 12.50, 4.84, 78.87, 70.72, 6.37, 97.30, 94.68, 37.45, 10.42},
      {70.94, 97.25, 15.16, 15.39, 57.27, 25.72, 81.94, 83.33, 85.91, 92.12},
      {47.62, 93.10, 29.13, 15.45, 6.88, 28.32, 35.61, 95.39, 79.68, 16.85},
      {13.99, 6.91, 93.83, 92.77, 89.40, 81.76, 42.83, 54.16, 88.39, 28.85}
  };

  NewDijkstras my_dijkstras;

  my_dijkstras.setUpAlgo(heightmap2, std::make_pair(0,0), std::make_pair(4,4), std::make_pair(0,0), 45.0, 1.0);
  std::vector<std::pair<int, int>> testPath = my_dijkstras.newDijkstras();

  for(int i = 0; i < testPath.size(); i++)
  {
    std::cout << testPath[i].first << " " << testPath[i].second << std::endl;
  }

  std::cout << "end simple test" << std::endl;


  //ALl fine after
  try {
    /* Initialize Command Line Interface */
    mempa::CLI commandLineInterface(argc, argv); /* CLI.cpp Object */
    commandLineInterface.displayInputs();

    mempa::DemHandler marsDemHandler(
        commandLineInterface.getGeotiffFilepath()); /* DemHandler.cpp Object */

    std::pair<int, int>
        imgStartCoordinates; /* Start coordinate for RoverSimulator */
    std::pair<int, int>
        imgGoalCoordinates; /* Goal coordinate for RoverSimulator */
    if (commandLineInterface.isGeoCRS()) {
      const std::pair<std::pair<double, double>, std::pair<double, double>>
          geoCoordinates =
              commandLineInterface
                  .getGeoCoordinates(); /* Geospatial double coordinates for
                                           transforming. */
      imgStartCoordinates =
          marsDemHandler.transformCoordinates(geoCoordinates.first);
      imgGoalCoordinates =
          marsDemHandler.transformCoordinates(geoCoordinates.second);
    } else if (commandLineInterface.isImgCRS()) {
      const std::pair<std::pair<int, int>, std::pair<int, int>> imgCoordinates =
          commandLineInterface.getImgCoordinates();
      imgStartCoordinates = imgCoordinates.first;
      imgGoalCoordinates = imgCoordinates.second;
    } else {
      throw std::runtime_error("Input CRS must be geospatial or image based.");
    }

    mempa::RoverSimulator marsSimulator(&marsDemHandler, imgStartCoordinates,
                                        imgGoalCoordinates);

    /* TODO: Change this out for D* Lite Algorithm! */
    NewDijkstras roverRoutingAlgorithm; /* Dijkstra's Algorithm */
    std::vector<std::pair<int, int>> routedPath = marsSimulator.runSimulator(
        &roverRoutingAlgorithm, commandLineInterface.getSlopeTolerance(),
        commandLineInterface.getBufferSize());

    // DEM

    Metrics metrics;
    metrics.analizePath(routedPath);

    std::unique_ptr<PathLogger> roverPathLogger =
        PathLogger::createLogger(commandLineInterface.getJSONFlag());
    roverPathLogger->logPath(commandLineInterface.getOutputFilename(),
                             routedPath, metrics);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
  return 0;
}

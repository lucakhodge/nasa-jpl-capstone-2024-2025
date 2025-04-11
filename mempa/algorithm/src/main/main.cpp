/* mempa::CLI */
#include "CLI.hpp"

/* mempa::DemHandler */
#include "../dem-handler/DemHandler.hpp"

/* mempa::RoverSimulator */
#include "../rover-simulator/RoverSimulator.hpp"

/* Dijkstras */
#include "../src/rover-pathfinding-module/dijkstras.hpp"

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
int main(int argc, char *argv[])
{
    try
    {
        /* Initialize Command Line Interface */
        mempa::CLI commandLineInterface(argc, argv); /* CLI.cpp Object */
        commandLineInterface.displayInputs();

        mempa::DemHandler marsDemHandler(commandLineInterface.getGeotiffFilepath()); /* DemHandler.cpp Object */

        std::pair<int, int> imgStartCoordinates; /* Start coordinate for RoverSimulator */
        std::pair<int, int> imgGoalCoordinates;  /* Goal coordinate for RoverSimulator */
        if (commandLineInterface.isGeoCRS())
        {
            const std::pair<std::pair<double, double>, std::pair<double, double>> geoCoordinates = commandLineInterface.getGeoCoordinates(); /* Geospatial double coordinates for
                                             transforming. */
            imgStartCoordinates = marsDemHandler.transformCoordinates(geoCoordinates.first);
            imgGoalCoordinates = marsDemHandler.transformCoordinates(geoCoordinates.second);
        }
        else if (commandLineInterface.isImgCRS())
        {
            const std::pair<std::pair<int, int>, std::pair<int, int>> imgCoordinates = commandLineInterface.getImgCoordinates();
            imgStartCoordinates = imgCoordinates.first;
            imgGoalCoordinates = imgCoordinates.second;
        }
        else
        {
            throw std::runtime_error("Input CRS must be geospatial or image based.");
        }

        mempa::RoverSimulator marsSimulator(&marsDemHandler, imgStartCoordinates, imgGoalCoordinates);

        /* TODO: Change this out for D* Lite Algorithm! */
        Dijkstras roverRoutingAlgorithm; /* Dijkstra's Algorithm */
        std::vector<std::pair<int, int>> routedPath = marsSimulator.runSimulator(&roverRoutingAlgorithm, commandLineInterface.getSlopeTolerance(), commandLineInterface.getBufferSize());

        Metrics metrics; /* Metrics */
        metrics.analizePath(routedPath);

        std::unique_ptr<PathLogger> roverPathLogger = PathLogger::createLogger(commandLineInterface.getJSONFlag());
        roverPathLogger->logPath(commandLineInterface.getOutputFilename(), routedPath, metrics);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}

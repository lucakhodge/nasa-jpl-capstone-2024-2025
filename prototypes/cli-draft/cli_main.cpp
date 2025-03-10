#include <iostream>
#include "cli.h"
//#include "buf_dem.h"
//#include "dijkstras.h"

int main(int argc, char* argv[]) {
    CLI config;

    config.parseArgs(argc, argv);

    config.validateInputs();

    if (!config.coordinate_check()) {
        std::cerr << "Error: Invalid coordinate configurations" << std::endl;
        return 1;
    }

    config.displayInputs();

    std::cout << "Program successfully initialized. Proceeding with processing..." << std::endl;

    //DEM
    //MEMPA::BUFFDEM mars_dem(input_filepath, output_filepath);

    config.processCoordinates();

    //mars_dem.demArea(config.coordinates, config.radius, 0.8);

    //Simulator
    // MEMPA::Simulator simulator(string n);

    // void run(int startX, int startY, int endX, int endY, std::string inFile)
    // ** can pass output file given my user ** or NULL for output into created .txt file
    // simulator.run(config.coordinates.front().first, config.coordinates.front().second, config.coordinates.back().first, config.coordinates.back().second, config.input_file);

    return 0;
}

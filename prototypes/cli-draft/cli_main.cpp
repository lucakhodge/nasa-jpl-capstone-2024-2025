#include <iostream>
#include "cli.h"
//#include "buf_dem.h"

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

    return 0;
}


/* C++ Standard Libraries */
#include <iostream>

/* Capstone Project Libraries */
// #include "../dem_handler/buf_dem.h"

/**
 * @brief Main function for terminal interface testing.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-10-16
 */
/*int main(int argc, const char *argv[])*/
/*{*/
/*    if (argc < 3)*/
/*    {*/
/*        std::cerr << "Error: Too few arguments provided.\nUsage: " << argv[0] << " <input_filepath> <output_filepath>" << std::endl;*/
/*        return 1;*/
/*    }*/
/**/
/*    const std::string_view input_filepath(argv[1]);*/
/*    const std::string_view output_filepath(argv[2]);*/
/**/
/*    try*/
/*    {*/
/*        MEMPA::BUFFDEM mars_dem(input_filepath, output_filepath);*/
/**/
/*        const std::pair<double, double> starting_coordinate(-7.25, -30.86);*/
/*        const std::pair<double, double> ending_coordinate(1.08, -28.44);*/
/*        const std::vector<std::pair<double, double>> coordinates = {starting_coordinate, ending_coordinate};*/
/*        mars_dem.demArea(coordinates, 0.0, 0.8);*/
/*        mars_dem.makeSHP("testshape", true);*/
/*        mars_dem.demClip("testclip", true);*/
/*        std::vector<std::vector<double>> myvec = mars_dem.demVector(*mars_dem.getOutput());*/
/*    }*/
/*    catch (const std::exception &e)*/
/*    {*/
/*        std::cerr << e.what() << std::endl;*/
/*        return 1;*/
/*    }*/
/**/
/*    return 0;*/
/*}*/

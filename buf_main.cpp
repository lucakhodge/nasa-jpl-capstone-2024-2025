/* C++ Standard Libraries */
#include <cstdlib>
#include <iostream>
#include <cctype>
#include <limits>

/* Capstone Project Libraries */
#include "buf_dem.h"
#include "buf_rover.h"

/**
 * @brief Displays menu for user to select modifications for DEM file.
 *
 *
 * @authors
 * Brock Hoos <brho7443@colorado.edu>
 * Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-10-15
 */
void dem_menu(MEMPA::BUF_DEM dem_file)
{
    char choice = '\0';
    while (true)
    {
        std::cout << R"(==== DEM File Processing ====
C - Chunk DEM File
F - Filter DEM File
M - Mask DEM File
Q - Continue
)";

        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        choice = static_cast<char>(std::toupper(choice));

        switch (choice)
        {
        case 'C':
            dem_file.dem_chunk();
            break;
        case 'F':
            dem_file.dem_filter();
            break;
        case 'M':
            dem_file.dem_mask();
            break;
        case 'Q':
            std::cout << "Exiting.\n";
            return;
        default:
            std::cerr << "Invalid Input.\n";
            break;
        }
    }
}

/**
 * @brief Main function for terminal interface.
 *
 * @author Ryan Wagster <ryan.wagster@colorado.edu>
 * @date 2024-10-16
 */
int main(int argc, char *argv[])
{
    const int expected_args = 3;
    if (argc < expected_args)
    {
        std::cerr << "Error: Too few arguments provided.\n"
                  << "Usage: " << argv[0] << " <input_filepath> <output_filepath>\n";
        return EXIT_FAILURE;
    }

    std::string_view input_filepath(argv[1]);
    std::string_view output_filepath(argv[2]);

    try
    {
        MEMPA::BUF_DEM dem_processor(input_filepath, output_filepath);
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Invalid Argument Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "Filesystem Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Runtime Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    // TODO
    // 1. Instantiate DEM object using user inputs.
    // Exit if catching errors.
    // 2. Launch menu.
    // 3. Destroy DEM object and return.

    return EXIT_SUCCESS;
}
/* C++ Standard Libraries */
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
        std::cout << R"(==== DEM File Modification Menu ====
        'C' - Chunk DEM File
        'F' - Filter DEM File
        'M' - Mask DEM File
        'Q' - Continue
        )";

        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        choice = static_cast<char>(std::toupper(choice));

        switch (choice)
        {
        case 'C':
            std::cout << "Chunking DEM File...\n";
            dem_file.dem_chunk();
            break;
        case 'F':
            std::cout << "Filtering DEM File...\n";
            dem_file.dem_filter();
            break;
        case 'M':
            std::cout << "Masking DEM File...\n";
            dem_file.dem_mask();
            break;
        case 'Q':
            std::cout << "Continuing...\n";
            return;
        default:
            std::cout << "Invalid Input. Try again.\n";
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
    // TODO
    // 1. Instantiate DEM object using user inputs.
    // Exit if catching errors.
    // 2. Launch menu.
    // 3. Destroy DEM object and return.

    return 0;
}
/* C++ Standard Libraries */
#include <iostream>
#include <cctype>

/* Capstone Project Libraries */
#include "buf_dem.h"
#include "buf_rover.h"

/**
 * @brief Displays menu for user to select modifications for DEM file
 *
 *
 * @author Brock Hoos <brho7443@colorado.edu.
 * @date 2024-10-15
 */
void dem_menu(MEMPA::BUF_DEM dem_file)
{
    char choice = '\0';

    while (TRUE)
    {
        std::cout << "==== DEM File Modification Menu ====\n"
                  << "'C' - Chunk DEM Fil\n"
                  << "'F' - Filter DEM File\n"
                  << "'M' - Mask DEM File\n"
                  << "'Q' - Continue" << std::endl;

        std::cin >> choice;
        choice = static_cast<char>(std::toupper(choice));

        // todo: cin error checking

        switch (choice)
        {
        case 'C':
            std::cout << "Chunking DEM File..." << std::endl;
            dem_file.dem_chunk();
            break;
        case 'F':
            std::cout << "Filtering DEM File..." << std::endl;
            dem_file.dem_filter();
            break;
        case 'M':
            std::cout << "Masking DEM File..." << std::endl;
            dem_file.dem_mask();
            break;
        case 'Q':
            std::cout << "Continuing..." << std::endl;
            return;
        default:
            std::cout << "Invalid Input. Try again" << std::endl;
            break;
        }
    }
}

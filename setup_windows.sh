#!/usr/bin/env bash

set -e

echo "Updating MSYS2 and installing packages..."
pacman -Syu --noconfirm
pacman -S --noconfirm \
    make \
    mingw-w64-x86_64-gcc \
    mingw-w64-x86_64-gdal \
    mingw-w64-x86_64-libtiff \
    mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-nlohmann-json

echo "Building the main program..."
cd mempa/algorithm
make

echo "Downloading Mars DEM .tif..."
cd tests
mkdir -p .
curl -L "https://planetarymaps.usgs.gov/mosaic/Mars/THEMIS_controlled_mosaics/SinusSabaeus_DayIR_31Jan2014/THEMIS_DayIR_ControlledMosaic_SinusSabaeus_30S00E_100mpp.tif" -o mars_dem.tif

echo "Running tests..."
cd ..
PRINT_OUTPUT="false" make test
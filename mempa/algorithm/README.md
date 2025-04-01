# Mars Rover Path Planning Simulator

## Building the Simulator

### Prerequisites
- g++ compiler
- GDAL library
- TIFF library
- TIFF file: https://astrogeology.usgs.gov/search/map/mars_mgs_mola_mex_hrsc_blended_dem_global_200m

### Compilation
```bash
# Using make (recommended)
make clean && make
```

2. Run the code using the following command:

```
Use the following command to run the simulator with default coordinates:
./simulator

Use the following command to run the simulator with specific coordinates:
./simulator --input <path/to/demFile> --start-pixel x,y --end-pixel x,y --iterations (int), --slope (double) --radius (int)

example:
./simulator --input /mnt/c/Users/Oscar/Desktop/MEMPA_Personal/Mars_HRSC_MOLA_BlendDEM_Global_200mp_v2.tif --start-pixel 1,1 --end-pixel 2000,2000 --iterations 1 --slope 45.0 --radius 20

WARNING:
currently there is a bug that if you are trying to move left or down it breaks so only do -> low,low to high,high.   
This will be fixed as ryan and or oscar make the simulator changes 
```

### Running Multiple Tests
```
# First build the simulator then:
cd /sim/tests
./run_tests.sh
# Output will be saved in /sim/test_results
```

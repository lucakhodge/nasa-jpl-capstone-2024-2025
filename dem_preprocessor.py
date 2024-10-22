"""
script to open up DEM file with python and print elevation + metadata

1. pip install rasterio
2. pip install numpy
2. Ensure correct path to dem
"""

import os
import rasterio
import numpy as np

with rasterio.open('data/Mars_HRSC_MOLA_BlendDEM_Global_200mp_v2.tif') as dem:
    print("################ ELEVATION DATA ################")
    
    # Read the elevation data as a numpy array
    elevation_data = dem.read(1)
    print("elevation_data:", elevation_data)
    print("HEIGHT:", len(elevation_data))
    print("WIDTH:", len(elevation_data[0]))

    elevation_data_file_path = "data/elevation_data.txt"
    if not os.path.exists(elevation_data_file_path):
        np.savetxt(elevation_data_file_path, elevation_data)

    print("################ METADATA ################")

    print(f"{dem.block_shapes=}")
    print(f"{dem.block_window=}")
    print(f"{dem.block_windows=}")
    print(f"{dem.bounds=}")
    print(f"{dem.checksum=}")
    print(f"{dem.close=}")
    print(f"{dem.closed=}")
    print(f"{dem.colorinterp=}")
    print(f"{dem.colormap=}")
    print(f"{dem.compression=}")
    print(f"{dem.count=}")
    print(f"{dem.crs=}")
    print(f"{dem.dataset_mask=}")
    print(f"{dem.descriptions=}")
    print(f"{dem.driver=}")
    print(f"{dem.dtypes=}")
    print(f"{dem.files=}")
    print(f"{dem.gcps=}")
    print(f"{dem.get_gcps=}")
    print(f"{dem.get_nodatavals=}")
    print(f"{dem.get_tag_item=}")
    print(f"{dem.get_transform=}")
    print(f"{dem.height=}")
    print(f"{dem.index=}")
    print(f"{dem.indexes=}")
    print(f"{dem.interleaving=}")
    print(f"{dem.is_tiled=}")
    print(f"{dem.lnglat=}")
    print(f"{dem.mask_flag_enums=}")
    print(f"{dem.meta=}")
    print(f"{dem.mode=}")
    print(f"{dem.name=}")
    print(f"{dem.nodata=}")
    print(f"{dem.nodatavals=}")
    print(f"{dem.offsets=}")
    print(f"{dem.options=}")
    print(f"{dem.overviews=}")
    print(f"{dem.photometric=}")
    print(f"{dem.profile=}")
    print(f"{dem.read=}")
    print(f"{dem.read_crs=}")
    print(f"{dem.read_masks=}")
    print(f"{dem.read_transform=}")
    print(f"{dem.res=}")
    print(f"{dem.rpcs=}")
    print(f"{dem.sample=}")
    print(f"{dem.scales=}")
    print(f"{dem.shape=}")
    print(f"{dem.start=}")
    print(f"{dem.stats=}")
    print(f"{dem.stop=}")
    print(f"{dem.subdatasets=}")
    print(f"{dem.tag_namespaces=}")
    print(f"{dem.tags=}")
    print(f"{dem.transform=}")
    print(f"{dem.units=}")
    print(f"{dem.width=}")
    print(f"{dem.window=}")
    print(f"{dem.window_bounds=}")
    print(f"{dem.window_transform=}")
    print(f"{dem.write_transform=}")
    print(f"{dem.xy=}")
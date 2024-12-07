import { GeoTIFF, fromFile } from "geotiff";
import { ChunkMapTile, ChunkMapTileCoordinate, DEMInfo } from "./electronIPC";

export class DEMManager {
  private chunkSize: number;
  private demWidth = 0;
  private demHeight = 0;
  private geoTiff: GeoTIFF | null = null;

  constructor (chunkSize: number) {
    //
    this.chunkSize = chunkSize;
  }
  async openDEM(filePath: string) {
    //TODO handle can't open 
    this.geoTiff = await fromFile(filePath); // Load the GeoTIFF file

    // Get the first image from the GeoTIFF file
    const image = await this.geoTiff.getImage();
    this.demWidth = image.getWidth();
    this.demHeight = image.getHeight();
    console.log("datatype", image.getBytesPerPixel())
  }
  closeDEM() {
    //
    this.geoTiff = null
  }
  getDEMInfo() {
    //
    const numChunksX = Math.ceil(this.demWidth / this.chunkSize);
    const numChunksY = Math.ceil(this.demHeight / this.chunkSize);

    const demInfo: DEMInfo = {
      numChunksX: numChunksX,
      numChunksY: numChunksY
    }

    return demInfo;

  }
  async getChunk(location: ChunkMapTileCoordinate) {
    if (!this.geoTiff) {
      console.error('DEM data is not loaded');
      return null;
    }

    // Calculate the area to read based on the chunk coordinates
    const startX = location.x * this.chunkSize;
    const startY = location.y * this.chunkSize;
    const endX = Math.min(startX + this.chunkSize, this.demWidth);
    const endY = Math.min(startY + this.chunkSize, this.demHeight);

    try {
      // Get the image and read the rasters for the specified region
      const image = await this.geoTiff.getImage();
      const rasterData = await image.readRasters({
        window: [startX, startY, endX, endY], // Define the region to read
      });
      const elevationData = rasterData[0]; // Assuming single-band image (one channel for elevation)
      if (typeof elevationData === "number") {
        throw new Error("Raster was a number");
      }

      // Convert the raw raster data into a 2D array
      const chunkData: number[][] = [];
      let index = 0;
      for (let y = startY; y < endY; y++) {
        const row: number[] = [];
        for (let x = startX; x < endX; x++) {
          row.push(elevationData[index++]);
        }
        chunkData.push(row);
      }

      const tile: ChunkMapTile = {
        coordinate: location,
        data: chunkData,
        width: this.chunkSize,
        height: this.chunkSize
      }
      return tile
    } catch (err) {
      console.error('Error reading chunk data:', err);
      return null;
    }

  }
}
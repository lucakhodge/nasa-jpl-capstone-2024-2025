import { GeoTIFF, fromFile } from "geotiff";
import { ChunkDescription, ChunkMapTile, DEMInfo } from "./electronIPC";

export class DEMManager {
  private demInfo: DEMInfo | null = null;
  private geoTiff: GeoTIFF | null = null;

  async openDEM(filePath: string) {
    //TODO handle can't open
    this.geoTiff = await fromFile(filePath); // Load the GeoTIFF file

    // Get the first image from the GeoTIFF file
    const image = await this.geoTiff.getImage();
    this.demInfo = {
      width: image.getWidth(),
      height: image.getHeight(),
    };
  }
  closeDEM() {
    //
    this.geoTiff = null;
    this.demInfo = null;
  }
  getDEMInfo() {
    return this.demInfo;
  }
  async getChunk(description: ChunkDescription) {
    if (!this.geoTiff) {
      console.error("DEM data is not loaded");
      return null;
    }

    // Calculate the area to read based on the chunk coordinates
    const startX = description.coordinate.x * description.chunkSize.width;
    const startY = description.coordinate.y * description.chunkSize.height;
    const endX = Math.min(
      startX + description.chunkSize.width,
      this.demInfo.width
    );
    const endY = Math.min(
      startY + description.chunkSize.height,
      this.demInfo.height
    );

    console.log(
      "width, height",
      description.chunkSize.width,
      description.chunkSize.height
    );

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

      // Calculate dimensions of the chunk
      const width = endX - startX;
      const height = endY - startY;

      console.log("elevationData", elevationData.length);

      // Convert the raw raster data into a 2D array
      const chunkData: number[][] = [];
      for (let y = 0; y < height; y++) {
        const row: number[] = [];
        for (let x = 0; x < width; x++) {
          // Calculate index in the flat array using row-major order
          const index = y * width + x;
          row.push(elevationData[index]);
        }
        chunkData.push(row);
      }

      const tile: ChunkMapTile = {
        chunkDescription: description,
        data: chunkData,
      };
      console.log("dimensions", tile.data.length, tile.data[0].length);
      return tile;
    } catch (err) {
      console.error("Error reading chunk data:", err);
      return null;
    }
  }
}

import { GeoTIFF, fromFile } from "geotiff";
import { ChunkDescription, Chunk, DEMInfo } from "./electronIPC";

export class DEMManager {
  private demInfo: DEMInfo | null = null;
  private geoTiff: GeoTIFF | null = null;

  async openDEM(filePath: string) {
    //TODO handle can't open
    this.geoTiff = await fromFile(filePath); // Load the GeoTIFF file

    // Get the first image from the GeoTIFF file
    const image = await this.geoTiff.getImage();
    const fileDirectory = image.getFileDirectory();
    const modelPixelScale = fileDirectory.ModelPixelScale;
    console.log("modelPixelScale", modelPixelScale);
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
  async getChunk(chunkDescription: ChunkDescription) {
    if (!this.geoTiff) {
      console.error("DEM data is not loaded");
      return null;
    }

    // Calculate the area to read based on the chunk coordinates
    const startX = chunkDescription.coordinate.x;
    const startY = chunkDescription.coordinate.y;
    const endX = chunkDescription.coordinate.x + chunkDescription.size.width;
    const endY = chunkDescription.coordinate.y + chunkDescription.size.height;

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

      const chunk: Chunk = {
        description: chunkDescription,
        data: chunkData,
      };
      console.log("dimensions", chunk.data.length, chunk.data[0].length);
      return chunk;
    } catch (err) {
      console.error("Error reading chunk data:", err);
      return null;
    }
  }
}

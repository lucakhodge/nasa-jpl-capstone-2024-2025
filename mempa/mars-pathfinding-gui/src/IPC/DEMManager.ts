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
    if (!this.geoTiff || !this.demInfo) {
      console.error("DEM data is not loaded");
      return null;
    }

    const maxNumberOfPoints = 1000 * 1000;
    const numberOfPoints = chunkDescription.dimensions.width * chunkDescription.dimensions.height;
    let resolutionScaling = 0;
    while (maxNumberOfPoints < numberOfPoints / 2 ** (1 + resolutionScaling)) {
      resolutionScaling++;
    }

    let incrementAmmount = 2 ** resolutionScaling;

    console.log("in get chunk", chunkDescription)
    // Calculate the area to read based on the chunk coordinates
    const buffer = 20 * incrementAmmount;
    const startX = Math.max(0, chunkDescription.coordinate.x - buffer);
    const startY = Math.max(0, chunkDescription.coordinate.y - buffer);
    const endX = Math.min(this.demInfo.width, chunkDescription.coordinate.x + chunkDescription.dimensions.width + 2 * buffer);
    const endY = Math.min(this.demInfo.height, chunkDescription.coordinate.y + chunkDescription.dimensions.height + 2 * buffer);

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

      console.log("INCREMNT", incrementAmmount)
      // Convert the raw raster data into a 2D array
      const chunkData: number[][] = [];
      for (let y = 0; y < height; y += incrementAmmount) {
        const row: number[] = [];
        for (let x = 0; x < width; x += incrementAmmount) {
          // Calculate index in the flat array using row-major order
          const index = y * width + x;
          row.push(elevationData[index]);
        }
        chunkData.push(row);
      }

      const chunk: Chunk = {
        description: {
          coordinate: { x: startX, y: startY },
          dimensions: { width: endX - startX, height: endY - startY }
        },
        resolutionScaling: resolutionScaling,
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

import { ipcMain } from "electron";
import GeoTIFF, { fromArrayBuffer, fromFile } from "geotiff";
import { LOAD_DEM } from "./electronIPC";

// ipcMain.handle(LOAD_DEM, async (event, filePath: string) => {
//   // Read and parse the DEM file
//   const tiff = await fromFile(filePath);
//   const image = await tiff.getImage();
  
//   const width = image.getWidth();
//   const height = image.getHeight();
//   const data = await image.readRasters(); // DEM data in an array
  
//   return { width, height, data };
// });


ipcMain.handle(LOAD_DEM, async (event, file: Electron.File) => {
  try {
    // Read the file content as a buffer
    // const fileBuffer = await file.arrayBuffer(); // Convert the file to a buffer
    const filePath = file.path;
    const fileBuffer = await fs.promises.readFile(filePath);
    
    // Load the TIFF file using GeoTIFF
    const tiff = await fromArrayBuffer(fileBuffer);
    const image = await tiff.getImage();
    const width = image.getWidth();
    const height = image.getHeight();
    const data = await image.readRasters();

    // Return the DEM data to the renderer process
    return { width, height, data };
  } catch (error) {
    console.error("Error loading DEM:", error);
    throw error; // Rethrow error to be caught in the renderer process
  }
});

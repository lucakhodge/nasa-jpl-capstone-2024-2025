import { ipcMain } from "electron";
import { ChunkMapTileCoordinate, CLOSE_DEM, GET_CHUNK, OPEN_DEM } from "./electronIPC";

ipcMain.on(OPEN_DEM, () => {
// display open file dialog (for only .tiff)
// try to open the file
// if good, pass to DEM Manager class
// send: on dem opened (send DEM Manger's info about the file )
})

ipcMain.on(CLOSE_DEM, () => {
// reset the DEM manager class
// send: on dem closed
})

ipcMain.on(GET_CHUNK, (event, props: ChunkMapTileCoordinate) => {
  // call DEM manger get chuck for that coordiante
  //set event.return (retval) to that chunk
})
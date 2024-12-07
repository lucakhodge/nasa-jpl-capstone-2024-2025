import { dialog, ipcMain, ipcRenderer } from "electron";
import { getRendererWindow } from "../main";
import { ChunkMapTileCoordinate, CLOSE_DEM, DEMInfo, GET_CHUNK, ON_DEM_OPENED, OPEN_DEM } from "./electronIPC";

ipcMain.on(OPEN_DEM, async () => {
// display open file dialog (for only .tiff)
  const result = await dialog.showOpenDialog( {
    properties: ['openFile'] // You can add other properties like 'multiSelections' if you want multiple file selections
  });

  const demInfo: DEMInfo = {
    numChunkX: 0,
    numChunkY: 0
  }
  const rw = getRendererWindow()
  console.log("RENDER WINDOW", rw)
  rw.webContents.send(ON_DEM_OPENED, demInfo)

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
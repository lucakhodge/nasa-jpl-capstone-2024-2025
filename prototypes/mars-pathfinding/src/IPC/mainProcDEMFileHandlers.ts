import { dialog, ipcMain } from "electron";
import { getRendererWindow } from "../main";
import { DEMManager } from "./DEMManager";
import { ChunkDescription, CLOSE_DEM, DEMInfo, GET_CHUNK, ON_DEM_CLOSED, ON_DEM_OPENED, OPEN_DEM } from "./electronIPC";

const myDEMManager: DEMManager = new DEMManager()

ipcMain.on(OPEN_DEM, async () => {
try {
  // display open file dialog (for only .tiff)\
  const result = await dialog.showOpenDialog({
    properties: ['openFile'],
    filters: [
      { name: 'GeoTIFF Files', extensions: ['tif', 'tiff'] }
    ]
  });

  if (result.canceled) {
    // handle cancled
    return;
  }
  // try to open the file
  // if good, pass to DEM Manager class
  const filePath = result.filePaths[0]
  myDEMManager.openDEM(filePath).then(() => {
    // send: on dem opened (send DEM Manger's info about the file )
    const demInfo: DEMInfo = myDEMManager.getDEMInfo()
    // console.log("DEMINFO:", demInfo)
    getRendererWindow().webContents.send(ON_DEM_OPENED, demInfo)
  })

} catch (err) {
  // handel error
  console.error('Error opening file dialog:', err);
}


})

ipcMain.on(CLOSE_DEM, () => {
// reset the DEM manager class
// send: on dem closed
  getRendererWindow().webContents.send(ON_DEM_CLOSED)
})

ipcMain.on(GET_CHUNK, (event, props: ChunkDescription) => {
  // call DEM manger get chuck for that coordiante
  //set event.returnValue to that chunk (this is what the renderer will get back)
  console.log("GETTING CHUNK", props)
   myDEMManager.getChunk(props).then((chunk) => {
    event.returnValue = chunk
  })
})
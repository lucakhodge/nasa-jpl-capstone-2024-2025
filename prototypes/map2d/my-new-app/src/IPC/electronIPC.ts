import { ipcRenderer } from "electron";

export interface DEMData {
  width: number;
  height: number;
  data: number[]; // Array of height values, or whatever format the data comes in
}

export const LOAD_DEM = "load-dem"

// export const loadDEM =  ipcRenderer.send(LOAD_DEM)
export const loadDEM = (filePath: string): Promise<DEMData> => {return ipcRenderer.invoke(LOAD_DEM, filePath)}
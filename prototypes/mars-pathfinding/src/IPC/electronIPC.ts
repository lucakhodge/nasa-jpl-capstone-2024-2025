import { ipcRenderer } from "electron";

export interface DEMData {
  width: number;
  height: number;
  data: number[]; // Array of height values, or whatever format the data comes in
}

export interface DEMInfo {
  width: number;
  height: number;
}

export interface ChunkMapTileCoordinate {
  x: number;
  y: number;
}

export interface ChunkSize {
  width: number;
  height: number;
}

export interface ChunkDescription {
  coordinate: ChunkMapTileCoordinate;
  chunkSize: ChunkSize;
}

export interface ChunkMapTile {
  chunkDescription: ChunkDescription;
  data: number[][];
}

// constants for message chanels
export const OPEN_DEM = "open-dem";
export const CLOSE_DEM = "close-dem";
export const GET_CHUNK = "get-chunk";
export const ON_DEM_OPENED = "on-dem-opened";
export const ON_DEM_CLOSED = "on-dem-closed";

// render -> main
export const openDEM = () => ipcRenderer.send(OPEN_DEM);
export const closeDEM = () => ipcRenderer.send(CLOSE_DEM);
export const getChunk = (chunk: ChunkDescription): ChunkMapTile | null =>
  ipcRenderer.sendSync(GET_CHUNK, chunk);

// main -> rendered
export const onDEMOpened = (
  callback: (event: Electron.IpcRendererEvent, props: DEMInfo) => void
) => ipcRenderer.on(ON_DEM_OPENED, callback);
export const onDEMClosed = (
  callback: (event: Electron.IpcRendererEvent) => void
) => ipcRenderer.on(ON_DEM_CLOSED, callback);

export const REACT_LOADING_DONE = "react-loaded";
export const CALL_CPP = "call-cpp";

export const setReactLoaded = () => ipcRenderer.send(REACT_LOADING_DONE);
export const callCpp = (message: string) =>
  ipcRenderer.invoke(CALL_CPP, message);

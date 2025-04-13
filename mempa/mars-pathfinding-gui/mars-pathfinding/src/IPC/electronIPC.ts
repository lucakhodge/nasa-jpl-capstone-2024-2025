import { ipcRenderer } from "electron";

// Add these new exports
export const getTempDirectory = () => ipcRenderer.invoke('get-temp-directory');
export const findMetricsFile = () => ipcRenderer.invoke('find-metrics-file');

export const READ_FILE = "read-file";
export const readFile = (filePath: string) => ipcRenderer.invoke(READ_FILE, filePath);

export interface DEMInfo {
  width: number;
  height: number;
}

export interface ChunkCoordinate {
  x: number;
  y: number;
}

export interface ChunkDimensions {
  width: number;
  height: number;
}

export interface ChunkDescription {
  coordinate: ChunkCoordinate;
  dimensions: ChunkDimensions;
}

export interface Chunk {
  description: ChunkDescription;
  resolutionScaleLeve: number;
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
export const getChunk = (chunk: ChunkDescription): Chunk | null =>
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

export interface Parameters {
  startCoordinate: Coordinate
  endCoordinate: Coordinate
  slope: number | null

}

export interface Coordinate {
  x: number | null,
  y: number | null
}
export const CALL_ALGORITHIM = "call-algorithim"
export const callAlgorithim = (parameters: Parameters) =>
  ipcRenderer.invoke(CALL_ALGORITHIM, parameters);


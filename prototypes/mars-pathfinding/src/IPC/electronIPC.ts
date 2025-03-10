import { ipcRenderer } from "electron";

export interface DEMInfo {
  width: number;
  height: number;
}

export interface ChunkCoordinate {
  x: number;
  y: number;
}

export interface ChunkSize {
  width: number;
  height: number;
}

export interface ChunkDescription {
  coordinate: ChunkCoordinate;
  size: ChunkSize;
}

export interface Chunk {
  description: {
    coordinate: {
      x: number;
      y: number;
    };
    size: {
      width: number;
      height: number;
    };
    filename: string;
  };
  data: number[][];
}

export interface PathfindingResult {
  path: Array<[number, number]>;
  metrics: {
    length: number;
    steps: number;
    maxSlope: number;
    avgSlope: number;
  };
}

export interface ElectronIPC {
  openDEM: (filePath?: string) => Promise<any>;
  closeDEM: () => Promise<boolean>;
  getChunk: (args: any) => Promise<Chunk>;
  onDEMOpened: (callback: (demInfo: any) => void) => void;
  onDEMClosed: (callback: () => void) => void;
  runPathfinding: (args: {
    demFile: string;
    startX: number;
    startY: number;
    endX: number;
    endY: number;
    maxSlope: number;
  }) => Promise<PathfindingResult>;
}

declare global {
  interface Window {
    electronIPC: ElectronIPC;
  }
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

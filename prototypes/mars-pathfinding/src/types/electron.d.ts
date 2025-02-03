import { ipcRenderer } from "electron";
import { REACT_LOADING_DONE, CALL_CPP } from "../IPC/electronIPC";

interface ElectronIPC {
  openDEM: () => void;
  closeDEM: () => void;
  getChunk: (chunk: ChunkDescription) => ChunkMapTile | null;
  onDEMOpened: (
    callback: (event: Electron.IpcRendererEvent, demInfo: DEMInfo) => void
  ) => void;
  onDEMClosed: (callback: (() => void) | undefined) => void;
  setReactLoaded: () => void;
  callCpp: (string: string) => Promise<string>;
}

declare global {
  interface Window {
    electronIPC: ElectronIPC;
  }
}

export {};

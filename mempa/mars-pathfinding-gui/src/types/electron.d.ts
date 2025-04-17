import { ipcRenderer } from "electron";
import { REACT_LOADING_DONE, CALL_CPP, Parameters, AlgorithimData } from "../IPC/electronIPC";

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
  // callAlgorithim: (Parameters: Parameters) => Promise<string>;
  callAlgorithim: (Parameters: Parameters) => void;
  onAlgorithimEnd: (callback: (event: Election.IpcRendererEvent, AlgorithimData) => void) => void;
}

declare global {
  interface Window {
    electronIPC: ElectronIPC;
  }
}

export { };

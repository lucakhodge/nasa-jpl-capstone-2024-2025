interface ElectronIPC {
  openDEM: () => void;
  closeDEM: () => void;
  getChunk: (chunk: ChunkDescription) => ChunkMapTile | null;
  onDEMOpened: (callback: (event: Electron.IpcRendererEvent, demInfo: DEMInfo) => void) => void;
  onDEMClosed: (callback: (() => void) | undefined) => void;
}


declare global {
  interface Window {
    electronIPC: ElectronIPC;
  }
}

export { };

// src/electron/preload.ts

import { contextBridge, ipcRenderer } from "electron";

export const backend = {
  nodeVersion: async (msg: string): Promise<string> =>
    await ipcRenderer.invoke("node-version", msg),
};

contextBridge.exposeInMainWorld("backend", backend);
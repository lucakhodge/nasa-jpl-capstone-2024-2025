// See the Electron documentation for details on how to use preload scripts:
// https://www.electronjs.org/docs/latest/tutorial/process-model#preload-scripts

import { contextBridge, ipcRenderer } from 'electron';

// Expose protected methods that allow the renderer process to use
// the ipcRenderer without exposing the entire object
contextBridge.exposeInMainWorld('electronIPC', {
  // Your existing methods
  openDEM: (filePath) => ipcRenderer.invoke('open-dem', filePath),
  closeDEM: () => ipcRenderer.invoke('close-dem'),
  getChunk: (args) => ipcRenderer.invoke('get-chunk', args),
  onDEMOpened: (callback) => ipcRenderer.on('on-dem-opened', (_, ...args) => callback(...args)),
  onDEMClosed: (callback) => ipcRenderer.on('on-dem-closed', (_, ...args) => callback(...args)),
  
  // Add the pathfinding method
  runPathfinding: (args) => ipcRenderer.invoke('run-pathfinding', args)
});
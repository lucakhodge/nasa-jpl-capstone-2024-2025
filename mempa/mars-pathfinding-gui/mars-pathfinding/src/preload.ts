// See the Electron documentation for details on how to use preload scripts:
// https://www.electronjs.org/docs/latest/tutorial/process-model#preload-scripts

import { contextBridge } from 'electron';
import { callAlgorithim, callCpp, closeDEM, getChunk, onDEMClosed, onDEMOpened, openDEM, setReactLoaded } from './IPC/electronIPC';


// contextBridge.exposeInMainWorld('electron', {
//   loadDEM: (filePath: string) => ipcRenderer.invoke('load-dem', filePath),
// });

contextBridge.exposeInMainWorld("electronIPC", { openDEM, closeDEM, getChunk, onDEMOpened, onDEMClosed, callCpp, setReactLoaded, callAlgorithim });

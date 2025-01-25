// See the Electron documentation for details on how to use preload scripts:
// https://www.electronjs.org/docs/latest/tutorial/process-model#preload-scripts

import { contextBridge } from 'electron';
import { closeDEM, getChunk, onDEMClosed, onDEMOpened, openDEM } from './IPC/electronIPC';


// contextBridge.exposeInMainWorld('electron', {
//   loadDEM: (filePath: string) => ipcRenderer.invoke('load-dem', filePath),
// });

contextBridge.exposeInMainWorld("electronIPC", {openDEM,closeDEM, getChunk, onDEMOpened, onDEMClosed });
// See the Electron documentation for details on how to use preload scripts:
// https://www.electronjs.org/docs/latest/tutorial/process-model#preload-scripts

import { contextBridge } from 'electron';
import { 
  callAlgorithim, 
  callCpp, 
  closeDEM, 
  getChunk, 
  onDEMClosed, 
  onDEMOpened, 
  openDEM, 
  readFile, // This should be from electronIPC, not fs
  setReactLoaded 
} from './IPC/electronIPC';

// IMPORTANT: Remove this import, it's conflicting with your electronIPC readFile
// import { readFile } from 'fs'; <-- REMOVE THIS LINE

contextBridge.exposeInMainWorld("electronIPC", { 
  openDEM, 
  closeDEM, 
  getChunk, 
  onDEMOpened, 
  onDEMClosed, 
  callCpp, 
  setReactLoaded, 
  callAlgorithim, 
  readFile 
});
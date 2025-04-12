import { contextBridge } from 'electron';
import { 
  callAlgorithim, 
  callCpp, 
  closeDEM, 
  getChunk, 
  onDEMClosed, 
  onDEMOpened, 
  openDEM, 
  setReactLoaded, 
  readFile,
  getTempDirectory,
  findMetricsFile 
} from './IPC/electronIPC';

// Make sure to expose all the functions
contextBridge.exposeInMainWorld("electronIPC", { 
  openDEM, 
  closeDEM, 
  getChunk, 
  onDEMOpened, 
  onDEMClosed, 
  callCpp, 
  setReactLoaded, 
  callAlgorithim, 
  readFile, 
  getTempDirectory, 
  findMetricsFile 
});
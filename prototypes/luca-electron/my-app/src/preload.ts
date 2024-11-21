// See the Electron documentation for details on how to use preload scripts:
// https://www.electronjs.org/docs/latest/tutorial/process-model#preload-scripts

import { contextBridge } from "electron";
import { callCpp, setReactLoaded } from "./IPC/electronIPC";

contextBridge.exposeInMainWorld("electronIPC", {setReactLoaded, callCpp});
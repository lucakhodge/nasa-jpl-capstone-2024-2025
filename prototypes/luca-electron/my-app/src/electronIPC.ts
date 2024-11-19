import { ipcRenderer } from "electron"

export const REACT_LOADING_DONE = "react-loaded"

export const setReactLoaded = () => ipcRenderer.send(REACT_LOADING_DONE)
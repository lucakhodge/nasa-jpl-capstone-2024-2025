import { ipcRenderer } from "electron"

export const REACT_LOADING_DONE = "react-loaded"
export const CALL_CPP = "call-cpp"

export const setReactLoaded = () => ipcRenderer.send(REACT_LOADING_DONE)

export const callCpp = () => ipcRenderer.invoke(CALL_CPP)

import { createSlice, PayloadAction } from "@reduxjs/toolkit"
import { RootState } from "./store"
import { Metrics, Path } from "../IPC/electronIPC";


interface MapState {
  loadState: LoadState,
  loadState: LoadState,
  path: Path | null,
  metrics: Metrics | null,
}

export type LoadState = 'idle' | 'loading' | 'loaded' | 'error';


const initialState: MapState = {
  loadState: 'idle',
  loadState: 'idle',
  path: null,
  metrics: null,
}

export const mapSlice = createSlice({
  name: "map",
  initialState,
  reducers: {
    setPath: (state, action: PayloadAction<Path>) => {
      state.path = action.payload;
    },
    setMetrics: (state, action: PayloadAction<Metrics>) => {
      state.metrics = action.payload;
    },
    setLoadState: (state, action: PayloadAction<LoadState>) => {
      state.loadState = action.payload;
    },
    setLoadState: (state, action: PayloadAction<LoadState>) => {
      state.loadState = action.payload;
    }
  },
})

export const { setPath, setMetrics, setLoadState } = mapSlice.actions
export const { setPath, setMetrics, setLoadState } = mapSlice.actions

export const selectPath = (state: RootState) => state.map.path
export const selectMetrics = (state: RootState) => state.map.metrics
export const selectLoadState = (state: RootState) => state.map.loadState
export const selectLoadState = (state: RootState) => state.map.loadState

export default mapSlice.reducer


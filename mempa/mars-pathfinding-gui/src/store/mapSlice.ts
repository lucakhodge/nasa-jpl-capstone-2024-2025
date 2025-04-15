
import { createSlice, PayloadAction } from "@reduxjs/toolkit"
import { RootState } from "./store"


interface MapState {
  path: Path | null,
  metrics: Metrics | null,
}

export type Path = Coordinate[];

interface Coordinate {
  x: number,
  y: number,
}

interface Metrics {
  totalDistance: number,
  elevationGain: number,
  elevationLoss: number,
  maxSlope: number,
  averageSlope: number,
  maxElevation: number,
  minElevation: number,
  baseElevation: number,
}

const initialState: MapState = {
  path: null,
  metrics: null,
}

export const mapSlice = createSlice({
  name: "map",
  initialState,
  reducers: {
    setPath: (state, action: PayloadAction<Coordinate[]>) => {
      state.path = action.payload;
    },
    setMetrics: (state, action: PayloadAction<Metrics>) => {
      state.metrics = action.payload;
    }
  },
})

export const { setPath, setMetrics } = mapSlice.actions

export const selectPath = (state: RootState) => state.map.path
export const selectMetrics = (state: RootState) => state.map.metrics

export default mapSlice.reducer


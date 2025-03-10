
import { createSlice, PayloadAction } from "@reduxjs/toolkit"
import { RootState } from "./store"


interface MapState {
  data: number[][] | null,
  path: Path | null,
}

export type Path = Coordinate[];

interface Coordinate {
  x: number,
  y: number,
}

const initialState: MapState = {
  data: null,
  path: null,
}

export const mapSlice = createSlice({
  name: "map",
  initialState,
  reducers: {
    setMap: (state, action: PayloadAction<number[][]>) => {
      state.data = action.payload;
    },
    setPath: (state, action: PayloadAction<Coordinate[]>) => {
      state.path = action.payload;
    }
  },
})

export const { setMap, setPath } = mapSlice.actions

export const selectMap = (state: RootState) => state.map.data
export const selectPath = (state: RootState) => state.map.path

export default mapSlice.reducer


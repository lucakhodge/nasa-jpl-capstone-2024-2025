import { createSlice, PayloadAction } from "@reduxjs/toolkit"

interface ParamatersState {
  startCoordinate: Coordinate
  endCoordinate: Coordinate
  slope: number | null
}

interface Coordinate {
  x: number | null,
  y: number | null
}

const initialState: ParamatersState = {
  startCoordinate: { x: null, y: null },
  endCoordinate: { x: null, y: null },
  slope: null
}

export const paramaterSlice = createSlice({
  name: "paramaters",
  initialState,
  reducers: {
    setStartCoordinate: (state, action: PayloadAction<Coordinate>) => {
      state.startCoordinate = action.payload
    },
    setEndCoordinate: (state, action: PayloadAction<Coordinate>) => {
      state.endCoordinate = action.payload
    },
    setSlope: (state, action: PayloadAction<number | null>) => {
      state.slope = action.payload
    }

  },
})

export const { setStartCoordinate, setEndCoordinate, setSlope } = paramaterSlice.actions

export const selectStartCoordinate = (state: { paramaters: ParamatersState }) => state.paramaters.startCoordinate
export const selectEndCoordinate = (state: { paramaters: ParamatersState }) => state.paramaters.endCoordinate
export const selectSlope = (state: { paramaters: ParamatersState }) => state.paramaters.slope

export default paramaterSlice.reducer

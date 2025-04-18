import { createSlice, PayloadAction } from "@reduxjs/toolkit"
import { RootState } from "./store"


interface ParamatersState {
  startCoordinate: Coordinate,
  endCoordinate: Coordinate,
  slope: number | null
  radius: number | null
}

interface Coordinate {
  x: number | null,
  y: number | null,
}

// const initialState: ParamatersState = {
//   startCoordinate: { x: null, y: null },
//   endCoordinate: { x: null, y: null },
//   slope: null,
//   radius: null
// }
const initialState: ParamatersState = {
  startCoordinate: { x: 40000, y: 29000 },
  endCoordinate: { x: 41000, y: 30000 },
  slope: 15,
  radius: 1000
}

export const paramaterSlice = createSlice({
  name: "parameters",
  initialState,
  reducers: {
    setStartCoordinate: (state, action: PayloadAction<Coordinate | null>) => {
      state.startCoordinate = action.payload
    },
    setEndCoordinate: (state, action: PayloadAction<Coordinate | null>) => {
      state.endCoordinate = action.payload
    },
    setSlope: (state, action: PayloadAction<number | null>) => {
      state.slope = action.payload
    },
    setRadius: (state, action: PayloadAction<number | null>) => {
      state.radius = action.payload
    }
  },
})

export const { setStartCoordinate, setEndCoordinate, setSlope, setRadius } = paramaterSlice.actions

export const selectStartCoordinate = (state: RootState) => state.paramaters.startCoordinate
export const selectEndCoordinate = (state: RootState) => state.paramaters.endCoordinate
export const selectSlope = (state: RootState) => state.paramaters.slope
export const selectRadius = (state: RootState) => state.paramaters.radius
export const selectParameters = (state: RootState) => state.paramaters

export default paramaterSlice.reducer

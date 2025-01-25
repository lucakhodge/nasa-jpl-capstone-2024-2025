// import { createSlice, PayloadAction } from '@reduxjs/toolkit'
import { createSlice, PayloadAction } from '@reduxjs/toolkit'
import { RootState } from './store'

// Define a type for the slice state
interface DEMState {
  demPath: string | null
}

// Define the initial state using that type
const initialState: DEMState = {
  demPath: null,
}

export const demSlice = createSlice({
  name: 'dem',
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    setDemPath: (state, action: PayloadAction<string | null>) => {
      //TODO: make this more robust
      state.demPath = action.payload
    },
  },
})

export const { setDemPath } = demSlice.actions

// Other code such as selectors can use the imported `RootState` type
export const selectDemPath = (state: RootState) => state.dem.demPath

export default demSlice.reducer
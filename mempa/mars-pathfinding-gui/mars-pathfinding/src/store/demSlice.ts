// import { createSlice, PayloadAction } from '@reduxjs/toolkit'
import { createSlice, PayloadAction } from '@reduxjs/toolkit'
import { DEMInfo } from '../IPC/electronIPC'
import { RootState } from './store'

// Define a type for the slice state
interface DEMState {
  demInfo: DEMInfo | null
}

// Define the initial state using that type
const initialState: DEMState = {
  demInfo: null,
}

export const demSlice = createSlice({
  name: 'dem',
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    setDemInfo: (state, action: PayloadAction<DEMInfo | null>) => {
      //TODO: make this more robust
      state.demInfo = action.payload
    },
  },
})

export const { setDemInfo } = demSlice.actions

// Other code such as selectors can use the imported `RootState` type
export const selectDemInfo = (state: RootState) => state.dem.demInfo

export default demSlice.reducer

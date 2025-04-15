import { createSlice, PayloadAction } from '@reduxjs/toolkit'
import { DEMInfo } from '../IPC/electronIPC'
import { RootState } from './store'

// Define a type for the slice state
interface DEMState {
  demInfo: DEMInfo | null;
  loading: boolean;
  error: string | null;
}

// Define the initial state using that type
const initialState: DEMState = {
  demInfo: null,
  loading: false,
  error: null
}

export const demSlice = createSlice({
  name: 'dem',
  // `createSlice` will infer the state type from the `initialState` argument
  initialState,
  reducers: {
    setDemInfo: (state, action: PayloadAction<DEMInfo | null>) => {
      state.demInfo = action.payload;
      state.error = null;
    },
    setDemLoading: (state, action: PayloadAction<boolean>) => {
      state.loading = action.payload;
    },
    setDemError: (state, action: PayloadAction<string | null>) => {
      state.error = action.payload;
      state.loading = false;
    },
    clearDem: (state) => {
      state.demInfo = null;
      state.error = null;
    }
  },
})

export const { setDemInfo, setDemLoading, setDemError, clearDem } = demSlice.actions

// Selectors
export const selectDemInfo = (state: RootState) => state.dem.demInfo;
export const selectDemLoading = (state: RootState) => state.dem.loading;
export const selectDemError = (state: RootState) => state.dem.error;
export const selectIsDemLoaded = (state: RootState) => state.dem.demInfo !== null;

export default demSlice.reducer
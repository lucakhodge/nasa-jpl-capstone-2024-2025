import { createSlice, PayloadAction } from '@reduxjs/toolkit';
import { RootState } from './store';

// Define the path data type
interface PathPoint {
  x: number;
  y: number;
}

// Define the slice state
interface PathState {
  path: PathPoint[] | null;
  loading: boolean;
  error: string | null;
}

const initialState: PathState = {
  path: null,
  loading: false,
  error: null
};

export const pathSlice = createSlice({
  name: 'path',
  initialState,
  reducers: {
    setPath: (state, action: PayloadAction<PathPoint[] | null>) => {
      state.path = action.payload;
      state.error = null;
    },
    setPathLoading: (state, action: PayloadAction<boolean>) => {
      state.loading = action.payload;
    },
    setPathError: (state, action: PayloadAction<string | null>) => {
      state.error = action.payload;
      state.loading = false;
    },
    clearPath: (state) => {
      state.path = null;
      state.error = null;
    }
  }
});

export const { setPath, setPathLoading, setPathError, clearPath } = pathSlice.actions;

// Selectors
export const selectPath = (state: RootState) => state.path.path;
export const selectPathLoading = (state: RootState) => state.path.loading;
export const selectPathError = (state: RootState) => state.path.error;

export default pathSlice.reducer;
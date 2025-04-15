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
  pathNotFound: boolean; // Added flag for path not found state
}

const initialState: PathState = {
  path: null,
  loading: false,
  error: null,
  pathNotFound: false // Initialize as false
};

export const pathSlice = createSlice({
  name: 'path',
  initialState,
  reducers: {
    setPath: (state, action: PayloadAction<PathPoint[] | null>) => {
      state.path = action.payload;
      state.error = null;
      state.pathNotFound = false; // Reset pathNotFound when setting a path
    },
    setPathLoading: (state, action: PayloadAction<boolean>) => {
      state.loading = action.payload;
      // Don't reset pathNotFound during loading
    },
    setPathError: (state, action: PayloadAction<string | null>) => {
      state.error = action.payload;
      state.loading = false;
      // Error is different from pathNotFound, so we don't set pathNotFound here
    },
    clearPath: (state) => {
      state.path = null;
      state.error = null;
      state.pathNotFound = false; // Reset pathNotFound when clearing path
    },
    setPathNotFound: (state, action: PayloadAction<boolean>) => {
      state.pathNotFound = action.payload;
      state.loading = false;
      // If path not found is true, ensure path is null
      if (action.payload === true) {
        state.path = null;
      }
    }
  }
});

export const { 
  setPath, 
  setPathLoading, 
  setPathError, 
  clearPath, 
  setPathNotFound 
} = pathSlice.actions;

// Selectors
export const selectPath = (state: RootState) => state.path.path;
export const selectPathLoading = (state: RootState) => state.path.loading;
export const selectPathError = (state: RootState) => state.path.error;
export const selectPathNotFound = (state: RootState) => state.path.pathNotFound;

export default pathSlice.reducer;
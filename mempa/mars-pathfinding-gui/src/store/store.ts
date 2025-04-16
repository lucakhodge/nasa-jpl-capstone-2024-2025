import { combineReducers, configureStore } from '@reduxjs/toolkit';
// import storage from "redux-persist/es/storage";
import sessionStorage from "redux-persist/es/storage/session";
import demReducer from './demSlice';
import paramaterReducer from './paramatersSlice';
import mapReducer from './mapSlice';
import {
  persistStore,
  persistReducer,
  FLUSH,
  REHYDRATE,
  PAUSE,
  PERSIST,
  PURGE,
  REGISTER,
} from 'redux-persist'

const rootReducer = combineReducers({
  dem: demReducer,
  paramaters: paramaterReducer,
  map: mapReducer,
})

const persistConfig = {
  key: "root",
  storage: sessionStorage, //persist on reload only, not on quit app
  whitelist: ["dem", "paramaters"] // Fixed typo: whilist -> whitelist
};

const persistedReducer = persistReducer(persistConfig, rootReducer);
export const store = configureStore({
  reducer: persistedReducer,
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware({
      serializableCheck: {
        ignoredActions: [FLUSH, REHYDRATE, PAUSE, PERSIST, PURGE, REGISTER],
      },
    }),
});

export const persistor = persistStore(store);


// Infer the `RootState` and `AppDispatch` types from the store itself
export type RootState = ReturnType<typeof store.getState>
// Inferred type: {posts: PostsState, comments: CommentsState, users: UsersState}
export type AppDispatch = typeof store.dispatch

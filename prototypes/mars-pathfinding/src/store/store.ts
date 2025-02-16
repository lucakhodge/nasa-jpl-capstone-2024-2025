import { combineReducers, configureStore } from '@reduxjs/toolkit';
import { persistReducer, persistStore } from 'redux-persist';
// import storage from "redux-persist/es/storage";
import sessionStorage from "redux-persist/es/storage/session";
import demReducer from './demSlice';
import paramaterReducer from './paramatersSlice';

const rootReducer = combineReducers({
  dem: demReducer,
  paramaters: paramaterReducer
})

const persistConfig = {
  key: "root",
  storage: sessionStorage, //persist on reload only, not on quit app
  whilist: ["dem", "paramaters"]
};

const persistedReducer = persistReducer(persistConfig, rootReducer);
export const store = configureStore({
  reducer: persistedReducer,
});

export const persistor = persistStore(store);


// Infer the `RootState` and `AppDispatch` types from the store itself
export type RootState = ReturnType<typeof store.getState>
// Inferred type: {posts: PostsState, comments: CommentsState, users: UsersState}
export type AppDispatch = typeof store.dispatch

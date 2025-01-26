import { configureStore } from '@reduxjs/toolkit'
// import demReducer from './demSlice'
import demReducer from './demSlice'


export const store = configureStore({
  reducer: {
    dem: demReducer
  }
})

// Infer the `RootState` and `AppDispatch` types from the store itself
export type RootState = ReturnType<typeof store.getState>
// Inferred type: {posts: PostsState, comments: CommentsState, users: UsersState}
export type AppDispatch = typeof store.dispatch
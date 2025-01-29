import { createRoot } from "react-dom/client";
import { Provider } from "react-redux";
import DemListener from "./components/DemListener";
import GetChunk from "./components/GetChunk";
import HeightChunkDisplay from "./components/HeightChunkDisplay";
import Hello from "./components/Hello";
import LoadFileButton from "./components/LoadFileButton";
import "./index.css";
import { store } from "./store/store";
import RunCpp from "./components/RunCpp";

const root = createRoot(document.body);
root.render(
  <div>
    <Provider store={store}>
      <DemListener />
      <Hello />
      <RunCpp />
      <div className="bg-red-700">in app</div>
      <LoadFileButton />
      <GetChunk />
    </Provider>
  </div>
);

// import React from "react";

// export default function App() {
//   return <div>App</div>;
// }

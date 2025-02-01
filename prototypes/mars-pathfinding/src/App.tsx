import { createRoot } from "react-dom/client";
import { Provider } from "react-redux";
import DemListener from "./components/DemListener";
import GetChunk from "./components/GetChunk";
import LoadFileButton from "./components/LoadFileButton";
import "./index.css";
import { store } from "./store/store";
import SampleThree from "./components/SampleThree";
import RawThree from "./components/RawThree";
const root = createRoot(document.body);
root.render(
  <div>
    <Provider store={store}>
      <DemListener />
      <LoadFileButton />
      {/* <RunCpp /> */}
      {/* <GetChunk /> */}
      {/* <SampleThree /> */}
      <RawThree />
      {/* <Map /> */}
    </Provider>
  </div>
);

// import React from "react";

// export default function App() {
//   return <div>App</div>;
// }

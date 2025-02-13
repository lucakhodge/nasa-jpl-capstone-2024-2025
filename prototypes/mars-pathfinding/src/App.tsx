import { createRoot } from "react-dom/client";
import { Provider } from "react-redux";
import DemListener from "./components/DemListener";
import LoadFileButton from "./components/LoadFileButton";
import RawThree from "./components/RawThree";
import { FigmaPage } from "./components/FigmaPage";
import "./index.css";
import { store } from "./store/store";
const root = createRoot(document.body);
root.render(
  <div>
    <Provider store={store}>
      <DemListener />
      {/* <LoadFileButton /> */}
      {/* <RunCpp /> */}
      {/* <GetChunk /> */}
      {/* <SampleThree /> */}
      {/* <RawThree /> */}
      {/* <Map /> */}
      <FigmaPage />
    </Provider>
  </div>
);

// import React from "react";

// export default function App() {
//   return <div>App</div>;
// }

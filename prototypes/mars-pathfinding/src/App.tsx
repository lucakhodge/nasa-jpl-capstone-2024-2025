import { createRoot } from "react-dom/client";
import { Provider } from "react-redux";
import DemListener from "./components/DemListener";
import LoadFileButton from "./components/LoadFileButton";
import Map from "./components/Map";
import "./index.css";
import { store } from "./store/store";
const root = createRoot(document.body);
root.render(
  <div>
    <Provider store={store}>
      <DemListener />
      <LoadFileButton />
      {/* <Hello />
      <RunCpp />
      <div className="bg-red-700">in app</div>
      <GetChunk /> */}
      <Map />
    </Provider>
  </div>
);

// import React from "react";

// export default function App() {
//   return <div>App</div>;
// }

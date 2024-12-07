import * as React from "react";
import { createRoot } from "react-dom/client";
import { Provider } from "react-redux";
import DEMListener from "./components/DEMListener";
import GetChunk from "./components/GetChunk";
import LoadFileButton from "./components/LoadFileButton";
import SampleRedux from "./components/SampleRedux";
import "./index.css";
import { store } from "./store/store";

const root = createRoot(document.body);
root.render(
  <Provider store={store}>
    <div>
      <h1 className="text-3xl font-bold underline text-emerald-500">
        Hello world!
      </h1>
      {/* <Maps></Maps> */}
      {/* <MyGeoTIff></MyGeoTIff> */}
      {/* <DEMMap /> */}
      <DEMListener></DEMListener>
      <LoadFileButton></LoadFileButton>
      <GetChunk></GetChunk>
      <SampleRedux />
    </div>
  </Provider>
);

import * as React from "react";
import { createRoot } from "react-dom/client";
import DEMListener from "./components/DEMListener";
import GetChunk from "./components/GetChunk";
import LoadFileButton from "./components/LoadFileButton";
import "./index.css";

const root = createRoot(document.body);
root.render(
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
  </div>
);

import * as React from "react";
import { createRoot } from "react-dom/client";
import GeoTiffHeightMap from "./GeoTiffHeightMap";
import "./index.css";
import Maps from "./Maps";

const root = createRoot(document.body);
root.render(
  <div>
    <h1 className="text-3xl font-bold underline text-emerald-500">
      Hello world!
    </h1>
    {/* <Maps></Maps> */}
    <GeoTiffHeightMap></GeoTiffHeightMap>
  </div>
);

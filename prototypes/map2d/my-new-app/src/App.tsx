import * as React from "react";
import { createRoot } from "react-dom/client";
import DEMMap from "./components/DEMMap";
import "./index.css";

const root = createRoot(document.body);
root.render(
  <div>
    <h1 className="text-3xl font-bold underline text-emerald-500">
      Hello world!
    </h1>
    {/* <Maps></Maps> */}
    {/* <MyGeoTIff></MyGeoTIff> */}
    <DEMMap />
  </div>
);

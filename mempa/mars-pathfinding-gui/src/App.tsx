import { createRoot } from "react-dom/client";
import { Provider } from "react-redux";
import DemListener from "./components/DemListener";
import "./index.css";
import { store } from "./store/store";
import MainFlow from "./flows/MainFlow";
import AlgorithimListener from "./components/AlgorithimLister";
const root = createRoot(document.body);
root.render(
  <div>
    <Provider store={store}>
      <DemListener />
      <AlgorithimListener />
      {/* <FigmaPage /> */}
      <MainFlow />
    </Provider>
  </div>
);

// import React from "react";

// export default function App() {
//   return <div>App</div>;
// }

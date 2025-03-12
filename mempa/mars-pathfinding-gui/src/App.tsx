import { createRoot } from "react-dom/client";
import { Provider } from "react-redux";
import DemListener from "./components/DemListener";
import FigmaPage from "./components/FigmaPage";
import "./index.css";
import { store } from "./store/store";
import MainFlow from "./components/MainFlow";
const root = createRoot(document.body);
root.render(
  <div>
    <Provider store={store}>
      <DemListener />
      {/* <FigmaPage /> */}
      <MainFlow />
    </Provider>
  </div>
);

// import React from "react";

// export default function App() {
//   return <div>App</div>;
// }

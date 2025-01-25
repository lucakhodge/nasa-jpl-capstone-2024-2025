import { createRoot } from "react-dom/client";
import { Provider } from "react-redux";
import DemPathDisplay from "./components/DemPathDisplay";
import DemPathSelect from "./components/DemPathSelect";
import Hello from "./components/Hello";
import "./index.css";
import { store } from "./store/store";

const root = createRoot(document.body);
root.render(
  <div>
    <Provider store={store}>
      <Hello />
      <div className="bg-red-700">in app</div>
      <DemPathSelect />
      <DemPathDisplay />
    </Provider>
  </div>
);

// import React from "react";

// export default function App() {
//   return <div>App</div>;
// }

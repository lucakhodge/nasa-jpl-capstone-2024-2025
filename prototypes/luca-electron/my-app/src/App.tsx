import { createRoot } from "react-dom/client";
import ReactReady from "./components/ReactReady";
import RunCpp from "./components/RunCpp";

const root = createRoot(document.body);
root.render(
  <>
    <ReactReady />
    <h2>Hello from React!</h2>
    <RunCpp />
  </>
);

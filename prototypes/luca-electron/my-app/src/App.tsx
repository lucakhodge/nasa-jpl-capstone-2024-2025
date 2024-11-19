import { createRoot } from "react-dom/client";
import ReactReady from "./ReactReady";

const root = createRoot(document.body);
root.render(
  <>
    <ReactReady />
    <h2>Hello from React!</h2>
  </>
);

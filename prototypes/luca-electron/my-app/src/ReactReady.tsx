import React, { useEffect } from "react";

export default function ReactReady() {
  useEffect(() => {
    console.log("calling reactloaded");
    window.electronIPC.setReactLoaded();
  }, []);

  return <></>;
}

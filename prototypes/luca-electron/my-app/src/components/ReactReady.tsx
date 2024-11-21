import React, { useEffect } from "react";

export default function ReactReady() {
  useEffect(() => {
    window.electronIPC.setReactLoaded();
  }, []);

  return <></>;
}

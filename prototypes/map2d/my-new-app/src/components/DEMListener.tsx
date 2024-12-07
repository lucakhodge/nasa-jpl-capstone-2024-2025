import React, { useEffect } from "react";
import { DEMInfo } from "../IPC/electronIPC";

export default function DEMListener() {
  useEffect(() => {
    window.electronIPC.onDEMOpened((mapInfo: DEMInfo) => {
      alert("go dem");
      // push data to redux store
    });
    window.electronIPC.onDEMClosed(() => {
      // push data to redux store
    });
    return () => {
      window.electronIPC.onDEMOpened(undefined);
      window.electronIPC.onDEMClosed(undefined);
    };
  }, []);

  return <></>;
}

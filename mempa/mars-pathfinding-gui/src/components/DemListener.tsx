import React, { useEffect } from "react";
import { DEMInfo } from "../IPC/electronIPC";
import { setDemInfo } from "../store/demSlice";
import { useAppDispatch } from "../store/hooks";

export default function DEMListener() {
  const dispatch = useAppDispatch();
  useEffect(() => {
    window.electronIPC.onDEMOpened(
      (_event: Electron.IpcRendererEvent, demInfo: DEMInfo) => {
        // push data to redux store
        console.log("demInfo", demInfo);
        dispatch(setDemInfo(demInfo));
      }
    );
    window.electronIPC.onDEMClosed(() => {
      // push data to redux store
      dispatch(null);
    });
    return () => {
      window.electronIPC.onDEMOpened(undefined);
      window.electronIPC.onDEMClosed(undefined);
    };
  }, []);

  return <></>;
}

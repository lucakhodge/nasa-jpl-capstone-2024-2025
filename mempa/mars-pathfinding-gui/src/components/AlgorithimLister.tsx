import { useEffect } from 'react'
import { AlgorithimData, Metrics, Path } from '../IPC/electronIPC';
import { useAppDispatch } from '../store/hooks';
import { setLoadState, setMetrics, setPath } from '../store/mapSlice';

export default function AlgorithimListener() {
  const dispatch = useAppDispatch();
  useEffect(() => {
    window.electronIPC.onAlgorithimEnd(
      (_event: Electron.IpcRendererEvent, algorithimData: AlgorithimData) => {
        console.log("In Algo Listener");
        if (algorithimData === null) {
          dispatch(setMetrics(null))
          dispatch(setPath(null))
          dispatch(setLoadState('error'));
        }
        else {
          dispatch(setMetrics(algorithimData.metrics))
          dispatch(setPath(algorithimData.path))
          dispatch(setLoadState('loaded'));
        }
      }
    );
    return () => {
      window.electronIPC.onAlgorithimEnd(undefined);
    };
  }, []);
  return (
    <></>
  )
}


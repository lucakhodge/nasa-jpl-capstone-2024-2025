import { useEffect, useRef, useState } from "react";
import { Chunk } from "../IPC/electronIPC";
import RegularThree from "../graphics/RegularThree";
import { MyButton } from "./MyButton";
import { Path } from "../store/mapSlice";

interface Map3dPropsI {
  chunk: Chunk | null
  path: Path | null
  edgeSmoothing?: number; // Add this line
}

export default function Map3d(props: Map3dPropsI) {
  const [regularThree, setRegularThree] = useState<RegularThree | null>(null);
  const canvasRef = useRef<HTMLCanvasElement>(null);

  // init three on load
  useEffect(() => {
    if (canvasRef.current) {
      setRegularThree(new RegularThree(canvasRef));
    } else {
      console.log("Could not find canvasRef");
    }
  }, []);

  ////update renderer when canvas is resized
  //useEffect(() => {
  //  if (regularThree) {
  //    const observer = new ResizeObserver(
  //      () => {
  //        // requestAnimationFrame(() => {
  //        //   regularThree?.resizeToCanvas();
  //        // });
  //      }
  //    );
  //    observer.observe(canvasRef.current);
  //    return () => {
  //      observer.disconnect();
  //    };
  //  }
  //}, [regularThree])

  // load chunk if chuck appears or changes
  // Only rerender once you have new chunk
  useEffect(() => {
    if (props.chunk && props.path) {
      console.log("ABOUT TO CALL DISPLAY CHUNK", props.chunk.resolutionScaling, props.chunk.description)
      regularThree?.displayChunk(props.chunk, props.path);
    }
  }, [props.chunk]);

  // toggle playback of the rover along path
  const handleTogglePlay = () => {
    regularThree?.togglePlay();
  }

  return (
    <div className="w-full h-full flex flex-col">
      <canvas ref={canvasRef} className="w-full h-full flex-grow" />
      <div className="p-4 text-center">
        <MyButton onClick={handleTogglePlay}>Play/Pause</MyButton>
        {/* <button onClick={() => { regularThree?.resizeToCanvas() }}>resize</button> */}
      </div>
    </div>
  );
}

import { useEffect, useRef, useState } from "react";
import { Chunk, Path } from "../IPC/electronIPC";
import RegularThree from "../graphics/RegularThree";
import { MyButton } from "./MyButton";

interface Map3dPropsI {
  chunk: Chunk | null
  path: Path | null
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
      <div className="relative flex-grow w-full min-w-0">
        <canvas ref={canvasRef} className="absolute w-full h-full" />
      </div>
      <div className="p-4 text-center">
        <MyButton onClick={handleTogglePlay}>Play/Pause</MyButton>
        {/* <button onClick={() => { regularThree?.resizeToCanvas() }}>resize</button> */}
      </div>
    </div>
  );
}

import { useEffect, useRef, useState } from "react";
import { Chunk } from "../IPC/electronIPC";
import RegularThree from "../graphics/RegularThree";
import { MyButton } from "./MyButton";

interface Map3dPropsI {
  chunk: Chunk | null
}

export default function Map3d(props: Map3dPropsI) {

  const [regularThree, setRegularThree] = useState<RegularThree | null>(null);
  const canvasRef = useRef<HTMLCanvasElement>(null);

  // init three on load
  useEffect(() => {
    if (canvasRef.current) {
      setRegularThree(new RegularThree(canvasRef.current));
    } else {
      console.log("nope");
    }
  }, []);

  // load chunk if chuck appears or changes
  useEffect(() => {
    if (props.chunk) {
      regularThree?.displayChunk(props.chunk);
    }
  }, [props.chunk]);

  // toggle playback of the rover along path
  const handleTogglePlay = () => {
    regularThree?.togglePlay();
  }

  return (
    <div>
      <canvas ref={canvasRef} style={{ width: "100%", height: "100%" }} />
      <MyButton onClick={handleTogglePlay}>Play/Pause</MyButton>
    </div>
  );
}

import { useEffect, useRef, useState } from "react";
import { Chunk } from "../IPC/electronIPC";
import RegularThree from "../graphics/RegularThree";

interface Map3dPropsI {
  chunk: Chunk | null;
  path?: Array<[number, number]>;  // Add path prop with proper type
}

export default function Map3d(props: Map3dPropsI) {
  const [regularThree, setRegularThree] = useState<RegularThree | null>(null);
  const canvasRef = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    if (canvasRef.current) {
      setRegularThree(new RegularThree(canvasRef.current));
    } else {
      console.log("nope");
    }
  }, []);

  useEffect(() => {
    if (props.chunk) {
      regularThree?.displayChunk(props.chunk);
    }
  }, [props.chunk]);

  useEffect(() => {
    if (props.path && regularThree) {
      regularThree.renderPath(props.path);
    }
  }, [props.path, regularThree]);

  return (
    <div className="w-full h-full">
      <canvas ref={canvasRef} style={{ width: "100%", height: "100%" }} />
    </div>
  );
}
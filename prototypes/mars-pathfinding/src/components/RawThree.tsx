import React, { useEffect, useRef, useState } from "react";
import RegularThree from "./RegularThree";
import { ChunkMapTile } from "../IPC/electronIPC";
export default function RawThree() {
  const [coordX, setCoordX] = useState(0);
  const [coordY, setCoordY] = useState(0);

  const [chunk, setChunk] = useState<ChunkMapTile | null>(null);
  const [regularThree, setRegularThree] = useState<RegularThree | null>(null);

  const handleClick = () => {
    console.log("yep");
    const chunk = window.electronIPC.getChunk({
      coordinate: {
        x: coordX,
        y: coordY,
      },
      chunkSize: {
        width: 500,
        height: 500,
      },
    });
    setChunk(chunk);
  };

  const canvasRef = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    if (canvasRef.current) {
      setRegularThree(new RegularThree(canvasRef.current));
    } else {
      console.log("nope");
    }
  }, []);

  useEffect(() => {
    if (chunk) {
      regularThree?.displayChunk(chunk);
    }
  }, [chunk]);

  return (
    <div>
      <input
        type="number"
        value={coordX}
        onChange={(e) => setCoordX(Number(e.target.value))}
      />
      <input
        type="number"
        value={coordY}
        onChange={(e) => setCoordY(Number(e.target.value))}
      />
      <button onClick={handleClick}>Click me</button>
      <div style={{ width: "80vw", height: "80vh" }}>
        <canvas ref={canvasRef} style={{ width: "100%", height: "100%" }} />
      </div>
    </div>
  );
}

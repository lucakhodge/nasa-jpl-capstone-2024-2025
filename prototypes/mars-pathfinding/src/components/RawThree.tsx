import React, { useEffect, useRef } from "react";
import RegularThree from "./RegularThree";
export default function RawThree() {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  useEffect(() => {
    if (canvasRef.current) {
      console.log("yep");
      const regularThree = new RegularThree(canvasRef.current);
    } else {
      console.log("nope");
    }
  }, []);

  return (
    <div
      className="border-2 border-red-500"
      style={{ width: "80vw", height: "80vh" }}
    >
      <canvas ref={canvasRef} style={{ width: "100%", height: "100%" }} />
    </div>
  );
}

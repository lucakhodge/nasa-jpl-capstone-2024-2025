import React, { useEffect, useRef } from "react";
import { ChunkMapTile } from "../IPC/electronIPC";

interface DisplayHeightPropsI {
  chunk: ChunkMapTile;
}

export default function DisplayHeight(props: DisplayHeightPropsI) {
  const canvasRef = useRef<HTMLCanvasElement>(null);

  const heightToColor = (height: number) => {
    // Map height to a color scale. This is a basic gradient.
    //TODO: dont hardcode minmax height, instead have it be part of the info
    const minHeight = -65536; // Minimum possible height value
    const maxHeight = 65536; // Maximum possible height value
    // const minHeight = -5000; // Minimum possible height value
    // const maxHeight = 0; // Maximum possible height value

    // Normalize the height to a value between 0 and 255
    const normalizedHeight =
      ((Math.min(Math.max(height, minHeight), maxHeight) - minHeight) /
        (maxHeight - minHeight)) *
      255;

    // Simple gradient from blue (low) to red (high)
    const red = Math.min(255, normalizedHeight);
    const green = Math.min(255, 255 - normalizedHeight);
    const blue = Math.min(255, 255 - normalizedHeight);
    // const blue = Math.floor(Math.random() * 256);
    // console.log(normalizedHeight, red, green, blue);

    return `rgb(${red}, ${green}, ${blue})`;
  };

  const renderChunk = (canvas: HTMLCanvasElement) => {
    const ctx = canvas.getContext("2d");
    if (!ctx) return;

    // Set canvas size based on the chunk dimensions
    canvas.width = props.chunk.width;
    canvas.height = props.chunk.height;

    // Loop through the height data and draw each pixel
    for (let row = 0; row < props.chunk.height; row++) {
      for (let col = 0; col < props.chunk.width; col++) {
        const heightValue = props.chunk.data[row][col];
        const color = heightToColor(heightValue);

        // Set the color and draw a single pixel
        ctx.fillStyle = color;
        ctx.fillRect(col, row, 1, 1); // Draw a 1x1 pixel
      }
    }
  };

  useEffect(() => {
    if (canvasRef.current) {
      renderChunk(canvasRef.current);
    }
  }, [props.chunk]);

  return <canvas ref={canvasRef} />;
}

import { useEffect, useRef } from "react";
import { ChunkMapTile } from "../IPC/electronIPC";

interface HeightChunkDisplayPropsI {
  chunk: ChunkMapTile;
}

//TODO: currently always displays a square, need to make it display the actual chunk size
export default function HeightChunkDisplay(props: HeightChunkDisplayPropsI) {
  console.log("HCD: props:", props);

  const canvasRef = useRef<HTMLCanvasElement>(null);

  const heightToColor = (
    height: number,
    maxHeight: number,
    minHeight: number
  ) => {
    // Map height to a color scale. This is a basic gradient.
    //TODO: dont hardcode minmax height, instead have it be part of the info
    // const minHeight = -65536; // Minimum possible height value
    // const maxHeight = 65536; // Maximum possible height value
    // const minHeight = -5000; // Minimum possible height value
    // const maxHeight = 0; // Maximum possible height value

    // Normalize the height to a value between 0 and 255
    const normalizedHeight =
      ((Math.min(Math.max(height, minHeight), maxHeight) - minHeight) /
        (maxHeight - minHeight)) *
      255;

    // Create rainbow gradient from black through colors to white
    let red, green, blue;
    if (normalizedHeight < 51) {
      // First fifth - black to blue
      const t = normalizedHeight / 51;
      red = 0;
      green = 0;
      blue = Math.floor(t * 255);
    } else if (normalizedHeight < 102) {
      // Second fifth - blue to green
      const t = (normalizedHeight - 51) / 51;
      red = 0;
      green = Math.floor(t * 255);
      blue = Math.floor((1 - t) * 255);
    } else if (normalizedHeight < 153) {
      // Third fifth - green to yellow
      const t = (normalizedHeight - 102) / 51;
      red = Math.floor(t * 255);
      green = 255;
      blue = 0;
    } else if (normalizedHeight < 204) {
      // Fourth fifth - yellow to red
      const t = (normalizedHeight - 153) / 51;
      red = 255;
      green = Math.floor((1 - t) * 255);
      blue = 0;
    } else {
      // Final fifth - red to white
      const t = (normalizedHeight - 204) / 51;
      red = 255;
      green = Math.floor(t * 255);
      blue = Math.floor(t * 255);
    }
    // const blue = Math.floor(Math.random() * 256);
    // console.log(normalizedHeight, red, green, blue);

    return `rgb(${red}, ${green}, ${blue})`;
  };

  const renderChunk = (canvas: HTMLCanvasElement) => {
    console.log("HCD: chunk:", props.chunk);

    const ctx = canvas.getContext("2d");
    if (!ctx) return;

    // Set canvas size to match chunk dimensions exactly
    canvas.width = props.chunk.chunkDescription.chunkSize.width;
    canvas.height = props.chunk.chunkDescription.chunkSize.height;

    const minHeight = -5000;
    const maxHeight = 5000;

    // Loop through the height data and draw each pixel
    for (
      let row = 0;
      row < props.chunk.chunkDescription.chunkSize.height;
      row++
    ) {
      for (
        let col = 0;
        col < props.chunk.chunkDescription.chunkSize.width;
        col++
      ) {
        const heightValue = props.chunk.data[row][col];
        const color = heightToColor(heightValue, maxHeight, minHeight);

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

  return <canvas className="border border-black" ref={canvasRef} />;
}

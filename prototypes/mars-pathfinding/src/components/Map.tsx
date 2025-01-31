import { useEffect, useState } from "react";
import { ChunkDescription, ChunkMapTile } from "../IPC/electronIPC";
import { selectDemInfo } from "../store/demSlice";
import { useAppSelector } from "../store/hooks";
import HeightChunkDisplay from "./HeightChunkDisplay";

export default function Map() {
  const demInfo = useAppSelector(selectDemInfo);

  const numAdditionalChunks = 2;

  const chunkSize = 1000;
  const [chunks, setChunks] = useState([]);

  const [lowCoordX, setLowCoordX] = useState(0);
  const [highCoordX, setHighCoordX] = useState(0);
  const [lowCoordY, setLowCoordY] = useState(0);
  const [highCoordY, setHighCoordY] = useState(0);

  useEffect(() => {
    addChunksInRange(
      lowCoordX - numAdditionalChunks,
      highCoordX + numAdditionalChunks,
      lowCoordY - numAdditionalChunks,
      highCoordY + numAdditionalChunks
    );
  }, [lowCoordX, highCoordX, lowCoordY, highCoordY]);

  const handleScroll = (event: React.UIEvent<HTMLDivElement>) => {
    const container = event.currentTarget;
    // Get scroll position plus half the container dimensions
    const lowX = container.scrollLeft;
    const highX = container.scrollLeft + container.clientWidth;
    const lowY = container.scrollTop;
    const highY = container.scrollTop + container.clientHeight;

    // Calculate coordinates based on scroll position and chunk size
    const lowCoordX = Math.floor(lowX / chunkSize);
    const highCoordX = Math.floor(highX / chunkSize);
    const lowCoordY = Math.floor(lowY / chunkSize);
    const highCoordY = Math.floor(highY / chunkSize);

    setLowCoordX(lowCoordX);
    setHighCoordX(highCoordX);
    setLowCoordY(lowCoordY);
    setHighCoordY(highCoordY);

    // addChunksInRange(lowCoordX, highCoordX, lowCoordY, highCoordY);
  };

  const getChunk = (coordX: number, coordY: number) => {
    const chunkDescription: ChunkDescription = {
      coordinate: { x: coordX, y: coordY },
      chunkSize: { width: chunkSize, height: chunkSize }, // TODO: make this dynamic, ALSO ERROR WHEN THEY ARE DIFFERENT
    };
    const newChunk = window.electronIPC.getChunk(chunkDescription);
    return newChunk;
    // console.log(newChunk);
    // if (newChunk !== null) {
    //   setChunks([...chunks, newChunk]);
    // }
  };

  const addChunksInRange = (
    lowCoordX: number,
    highCoordX: number,
    lowCoordY: number,
    highCoordY: number
  ) => {
    const newChunks: ChunkMapTile[] = [];
    for (let x = lowCoordX; x <= highCoordX; x++) {
      for (let y = lowCoordY; y <= highCoordY; y++) {
        const hasChunk = chunks.some(
          (chunk) =>
            chunk.chunkDescription.coordinate.x === x &&
            chunk.chunkDescription.coordinate.y === y
        );
        if (!hasChunk) {
          const newChunk = getChunk(x, y);
          if (newChunk !== null) {
            newChunks.push(newChunk);
          }
        } else {
          const existingChunk = chunks.find(
            (chunk) =>
              chunk.chunkDescription.coordinate.x === x &&
              chunk.chunkDescription.coordinate.y === y
          );
          if (existingChunk) {
            newChunks.push(existingChunk);
          }
        }
      }
    }
    setChunks([...newChunks]);
  };

  return demInfo !== null ? (
    <div>
      <div>
        <div
          style={{
            position: "relative",
            width: "2500px", // Set a fixed width
            height: "1500px", // Set a fixed height
            overflow: "auto", // Enable scrolling
            border: "1px solid #ccc", // Optional: adds a border to see the container bounds
          }}
          onScroll={handleScroll}
        >
          <div style={{ width: demInfo.width, height: demInfo.height }}></div>
          {chunks.map((chunk, index) => (
            <div
              key={index}
              style={{
                position: "absolute",
                left: `${chunk.chunkDescription.coordinate.x * chunkSize}px`,
                top: `${chunk.chunkDescription.coordinate.y * chunkSize}px`,
              }}
            >
              <HeightChunkDisplay chunk={chunk} />
            </div>
          ))}
        </div>
      </div>
    </div>
  ) : (
    <div>first select a dem file</div>
  );
}

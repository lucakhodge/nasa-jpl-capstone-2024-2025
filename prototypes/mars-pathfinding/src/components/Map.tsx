import { useState } from "react";
import { ChunkDescription } from "../IPC/electronIPC";
import { selectDemInfo } from "../store/demSlice";
import { useAppSelector } from "../store/hooks";
import HeightChunkDisplay from "./HeightChunkDisplay";

export default function Map() {
  const demInfo = useAppSelector(selectDemInfo);

  const [coordX, setCoordX] = useState<number | string>("");
  const [coordY, setCoordY] = useState<number | string>("");

  const chunkSize = 1000;
  const [chunks, setChunks] = useState([]);

  const handleXChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const value = event.target.value;
    setCoordX(value === "" ? "" : parseFloat(value));
  };
  const handleYChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const value = event.target.value;
    setCoordY(value === "" ? "" : parseFloat(value));
  };

  const addChunk = (coordX: number, coordY: number) => {
    const chunkDescription: ChunkDescription = {
      coordinate: { x: coordX, y: coordY },
      chunkSize: { width: chunkSize, height: chunkSize }, // TODO: make this dynamic, ALSO ERROR WHEN THEY ARE DIFFERENT
    };
    const newChunk = window.electronIPC.getChunk(chunkDescription);
    console.log(newChunk);
    if (newChunk !== null) {
      setChunks([...chunks, newChunk]);
    }
  };

  return demInfo !== null ? (
    <div>
      <div>
        <label>
          XCoord:
          <input
            type="number"
            value={coordX}
            onChange={handleXChange}
            placeholder="Enter X coordinate"
          />
        </label>
      </div>
      <div>
        <label>
          YCoord:
          <input
            type="number"
            value={coordY}
            onChange={handleYChange}
            placeholder="Enter Y coordinate"
          />
        </label>
      </div>
      <button
        onClick={() => {
          if (typeof coordX === "number" && typeof coordY === "number") {
            addChunk(coordX, coordY);
          }
        }}
      >
        Get Chunk
      </button>
      <div>
        <div
          style={{
            position: "relative",
            width: "2500px", // Set a fixed width
            height: "1500px", // Set a fixed height
            overflow: "auto", // Enable scrolling
            border: "1px solid #ccc", // Optional: adds a border to see the container bounds
          }}
          onScroll={(e) => {
            const container = e.currentTarget;
            // Get scroll position plus half the container dimensions
            const scrollX = container.scrollLeft + container.clientWidth / 2;
            const scrollY = container.scrollTop + container.clientHeight / 2;

            // Calculate coordinates based on scroll position and chunk size
            const coordX = Math.floor(scrollX / chunkSize);
            const coordY = Math.floor(scrollY / chunkSize);

            setCoordX(coordX);
            setCoordY(coordY);

            // Check if we don't already have a chunk at these coordinates
            const chunkExists = chunks.some(
              (existingChunk) =>
                existingChunk.chunkDescription.coordinate.x === coordX &&
                existingChunk.chunkDescription.coordinate.y === coordY
            );
            if (!chunkExists) {
              addChunk(coordX, coordY);
            }
            console.log(`Scrolled to coordinates: (${coordX}, ${coordY})`);
          }}
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

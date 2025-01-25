import React, { useState } from "react";
import { ChunkMapTile, ChunkMapTileCoordinate } from "../IPC/electronIPC";
import HeightChunkDisplay from "./HeightChunkDisplay";

export default function GetChunk() {
  const [coordX, setCoordX] = useState<number | string>("");
  const [coordY, setCoordY] = useState<number | string>("");
  const [chunk, setChunk] = useState<ChunkMapTile | null>(null);

  const handleXChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const value = event.target.value;
    setCoordX(value === "" ? "" : parseFloat(value));
  };
  const handleYChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const value = event.target.value;
    setCoordY(value === "" ? "" : parseFloat(value));
  };

  const handleGetChunk = () => {
    if (typeof coordX === "number" && typeof coordY === "number") {
      const chunkCoordinate: ChunkMapTileCoordinate = {
        x: coordX,
        y: coordY,
      };
      const newChunk = window.electronIPC.getChunk(chunkCoordinate);
      // alert("Got chunk");
      console.log(newChunk);
      setChunk(newChunk);
    } else {
      alert("Enter coordinates first");
    }
  };

  return (
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
      <button onClick={handleGetChunk}>Get Chunk</button>
      {/* <div>{JSON.stringify(chunk)}</div> */}
      {chunk !== null ? (
        <HeightChunkDisplay chunk={chunk}></HeightChunkDisplay>
      ) : (
        <div>No chunk</div>
      )}
    </div>
  );
}

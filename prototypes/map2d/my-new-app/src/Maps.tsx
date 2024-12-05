import React from "react";
import KonvaHeightMapWithPath from "./KonvaHeightMapWithPath";
import KonvaImage from "./KonvaImage";

const generateHeightMap = (width: number, height: number): number[][] => {
  const heightMap: number[][] = [];
  for (let row = 0; row < height; row++) {
    const rowData: number[] = [];
    for (let col = 0; col < width; col++) {
      rowData.push(Math.floor(Math.random() * 101)); // Random height between 0 and 100
    }
    heightMap.push(rowData);
  }
  return heightMap;
};

// Function to generate a sample path (random points across the height map)
const generateSamplePath = (
  width: number,
  height: number,
  numPoints: number
): { x: number; y: number }[] => {
  const path: { x: number; y: number }[] = [];
  for (let i = 0; i < numPoints; i++) {
    const x = Math.floor(Math.random() * width);
    const y = Math.floor(Math.random() * height);
    path.push({ x, y });
  }
  return path;
};

export default function Maps() {
  const width = 1000; // Width of the height map
  const height = 1000; // Height of the height map
  const numPoints = 5; // Number of points in the path
  const heightMap = generateHeightMap(width, height);
  const path = generateSamplePath(width, height, numPoints);
  return (
    <div>
      <div>Maps</div>
      <KonvaHeightMapWithPath
        heightData={heightMap}
        path={path}
      ></KonvaHeightMapWithPath>
      <KonvaImage heightData={heightMap} path={path}></KonvaImage>
    </div>
  );
}

import React, { useEffect, useRef, useState } from "react";
import { Circle, Image, Layer, Line, Stage } from "react-konva";

// Function to map height values to grayscale
const getColorForHeight = (height: number): string => {
  const colorValue = Math.min(255, Math.max(0, height)); // Ensure height is between 0 and 255
  return `rgb(${colorValue}, ${colorValue}, ${colorValue})`;
};

interface KonvaImageProps {
  heightData: number[][];
  path: { x: number; y: number }[];
}

const scaleFactor = 1;

const KonvaImage = ({ heightData, path }: KonvaImageProps) => {
  const containerRef = useRef<HTMLDivElement>(null); // Ref for the container div
  const [stageWidth, setStageWidth] = useState<number>(0); // Stage width based on container size
  const [stageHeight, setStageHeight] = useState<number>(0); // Stage height based on container size
  const [image, setImage] = useState<HTMLImageElement | null>(null); // Image to represent the height map

  // Update the stage size whenever the container size changes
  const updateStageSize = () => {
    if (containerRef.current) {
      const containerWidth = containerRef.current.offsetWidth;
      const containerHeight = containerRef.current.offsetHeight;
      setStageWidth(containerWidth);
      setStageHeight(containerHeight);
    }
  };

  useEffect(() => {
    // Initial size update
    updateStageSize();

    // Update stage size on window resize
    window.addEventListener("resize", updateStageSize);
    return () => {
      window.removeEventListener("resize", updateStageSize);
    };
  }, []);

  useEffect(() => {
    // Create a canvas for the height map
    const canvas = document.createElement("canvas");
    canvas.width = heightData[0].length; // Width of the height map
    canvas.height = heightData.length; // Height of the height map
    const ctx = canvas.getContext("2d");

    if (ctx) {
      // Draw the height map on the canvas
      for (let row = 0; row < heightData.length; row++) {
        for (let col = 0; col < heightData[0].length; col++) {
          const color = getColorForHeight(heightData[row][col]);
          ctx.fillStyle = color;
          ctx.fillRect(col, row, 1, 1); // 1x1 pixels for each cell
        }
      }

      const img = new window.Image() as HTMLImageElement;
      img.onload = () => setImage(img); // Set the image once it's loaded
      img.src = canvas.toDataURL(); // Create a data URL for the image
    }
  }, [heightData]);

  return (
    <div
      ref={containerRef}
      style={{
        width: "100%",
        height: "100%",
        position: "relative",
        overflow: "hidden",
      }}
    >
      <div>Konva Image</div>
      <Stage width={stageWidth} height={stageHeight}>
        <Layer>
          {/* Draw the height map as a single image */}
          {image && (
            <Image
              image={image}
              x={0}
              y={0}
              width={heightData[0].length * scaleFactor}
              height={heightData.length * scaleFactor}
            />
          )}

          {/* Draw the path as a red line */}
          <Line
            points={path.flatMap(({ x, y }) => [
              x * scaleFactor,
              y * scaleFactor,
            ])}
            stroke="red"
            strokeWidth={4}
            lineJoin="round"
            lineCap="round"
          />

          {/* Draw points (blue circles) along the path */}
          {path.map((point, index) => (
            <Circle
              key={index}
              x={point.x * scaleFactor}
              y={point.y * scaleFactor}
              radius={8}
              fill="blue"
            />
          ))}
        </Layer>
      </Stage>
    </div>
  );
};

export default KonvaImage;

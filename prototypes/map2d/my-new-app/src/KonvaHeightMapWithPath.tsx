import { Circle, Layer, Line, Rect, Stage } from "react-konva";

const getColorForHeight = (height: number): string => {
  // Map height to a grayscale value (0-255)
  const colorValue = Math.min(255, Math.max(0, height));
  return `rgb(${colorValue}, ${colorValue}, ${colorValue})`;
};

interface KonvaHeightMapWithPathProps {
  heightData: number[][];
  path: { x: number; y: number }[];
}

const scaleFactor = 1;

const KonvaHeightMapWithPath = ({
  heightData,
  path,
}: KonvaHeightMapWithPathProps) => {
  return (
    <Stage
      width={heightData[0].length * scaleFactor}
      height={heightData.length * scaleFactor}
    >
      <Layer>
        {/* Draw Height Map */}
        {heightData.map((row, rowIndex) =>
          row.map((height, colIndex) => (
            <Rect
              key={`${rowIndex}-${colIndex}`}
              x={colIndex * scaleFactor}
              y={rowIndex * scaleFactor}
              width={1 * scaleFactor}
              height={1 * scaleFactor}
              fill={getColorForHeight(height)}
            />
          ))
        )}

        {/* Draw Path */}
        <Line
          points={path.flatMap(({ x, y }) => [
            x * scaleFactor,
            y * scaleFactor,
          ])}
          stroke="red"
          strokeWidth={2}
          lineJoin="round"
          lineCap="round"
        />

        {/* Draw Points */}
        {path.map((point, index) => (
          <Circle
            key={index}
            x={point.x * scaleFactor}
            y={point.y * scaleFactor}
            radius={5}
            fill="blue"
          />
        ))}
      </Layer>
    </Stage>
  );
};
export default KonvaHeightMapWithPath;

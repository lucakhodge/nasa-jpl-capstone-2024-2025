import { fromBlob, fromBlob, fromFile } from "geotiff";
import { useRef, useState } from "react";

const renderHeightMap = (raster: any, width: number, height: number) => {
  const canvas = document.createElement("canvas");
  canvas.width = width;
  canvas.height = height;
  const ctx = canvas.getContext("2d");
  const imageData = ctx.createImageData(width, height);
  const data = imageData.data;

  // Map the height values to a color (grayscale example)
  for (let i = 0; i < raster[0].length; i++) {
    const heightValue = raster[0][i]; // Assuming raster[0] contains height data
    const color = Math.min(255, Math.max(0, heightValue)); // Normalize to 0-255 for grayscale
    data[i * 4] = color; // R
    data[i * 4 + 1] = color; // G
    data[i * 4 + 2] = color; // B
    data[i * 4 + 3] = 255; // A (full opacity)
  }

  ctx.putImageData(imageData, 0, 0);
  return canvas;
};

// // Function to read the DEM file and extract height data
// const readDemFile = async (file: File) => {
//   // const tiff = await fromFile(filePath);
//   const blob = file.slice(0, file.size); // Create a Blob from the file
//   const tiff = await fromBlob(blob); // Use `fromBlob` instead of `fromFile`
//   // const blob = new Blob([reader.result as ArrayBuffer], { type: "image/tiff" });
//   // const tiff = await GeoTIFF.fromBlob(blob);  // Load the TIFF file from the Blob
//   const image = await tiff.getImage();
//   const width = image.getWidth();
//   const height = image.getHeight();
//   const raster = await image.readRasters();
//   return { raster, width, height };
// };

const readDemFile = async (file: File) => {
  const blob = file.slice(0, file.size); // Create a Blob from the file
  const tiff = await fromBlob(blob); // Use `fromBlob` instead of `fromFile`
  const image = await tiff.getImage();
  const width = image.getWidth();
  const height = image.getHeight();

  // Instead of loading the entire raster, load in chunks
  const chunkWidth = 256; // Example chunk size
  const chunkHeight = 256;

  const chunk = await image.getData({
    window: [0, 0, chunkWidth, chunkHeight], // Load a chunk of data
  });

  return { raster: chunk, width, height };
};

const GeoTiffHeightMap = () => {
  const [heightMapData, setHeightMapData] = useState<any>(null);
  const canvasRef = useRef<HTMLCanvasElement | null>(null); // Reference for canvas

  const handleFileUpload = async (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (file) {
      const data = await readDemFile(file);
      setHeightMapData(data);
    }
  };

  // Render height map when the heightMapData is available
  const renderHeightMapCanvas = () => {
    if (heightMapData) {
      const { raster, width, height } = heightMapData;
      const canvas = renderHeightMap(raster, width, height);
      if (canvasRef.current) {
        canvasRef.current.replaceWith(canvas); // Replace old canvas with new one
        canvasRef.current = canvas; // Update the reference
      } else {
        return <canvas ref={canvasRef} width={width} height={height} />;
      }
    }
  };

  return (
    <div>
      <input type="file" accept=".tif" onChange={handleFileUpload} />
      {heightMapData && (
        <div>
          <h3>
            Height Map Dimensions: {heightMapData.width} x{" "}
            {heightMapData.height}
          </h3>
          {/* Render the height map here using the heightMapData */}
          {renderHeightMapCanvas()}
        </div>
      )}
    </div>
  );
};

export default GeoTiffHeightMap;

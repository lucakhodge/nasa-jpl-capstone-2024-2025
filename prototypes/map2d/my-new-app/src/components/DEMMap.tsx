import { fromFile } from "geotiff";
import L from "leaflet";
import "leaflet/dist/leaflet.css";
import React, { useEffect, useRef, useState } from "react";

interface DEMMapProps {}

const DEMMap: React.FC<DEMMapProps> = () => {
  const [demFile, setDemFile] = useState<File | null>(null);
  const [map, setMap] = useState<L.Map | null>(null);
  const mapRef = useRef<HTMLDivElement>(null);

  // Initialize Leaflet map
  useEffect(() => {
    if (mapRef.current && !map) {
      const newMap = L.map(mapRef.current, {
        center: [0, 0],
        zoom: 2,
        layers: [
          L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"),
        ],
      });

      setMap(newMap);
    }
  }, [map]);

  // Handle file upload
  const handleFileUpload = async (
    event: React.ChangeEvent<HTMLInputElement>
  ) => {
    const file = event.target.files?.[0];
    if (file) {
      setDemFile(file);

      try {
        // Load the TIFF file using geotiff.js
        const tiff = await fromFile(file);
        const image = await tiff.getImage();
        const rasters = await image.readRasters();

        // Assuming that the DEM data is in the form of an elevation array
        const width = image.getWidth();
        const height = image.getHeight();

        // Now we can create an image to overlay on the map.
        const canvas = document.createElement("canvas");
        const ctx = canvas.getContext("2d");
        if (ctx) {
          canvas.width = width;
          canvas.height = height;

          // Example: Map DEM elevation to grayscale for visualization
          const imageData = ctx.createImageData(width, height);
          for (let i = 0; i < rasters[0].length; i++) {
            const value = rasters[0][i]; // Elevation value
            const color = Math.min(255, value); // Normalize to 255 range
            imageData.data[i * 4 + 0] = color; // Red channel
            imageData.data[i * 4 + 1] = color; // Green channel
            imageData.data[i * 4 + 2] = color; // Blue channel
            imageData.data[i * 4 + 3] = 255; // Alpha channel
          }

          ctx.putImageData(imageData, 0, 0);

          // Create a Leaflet image overlay
          if (map) {
            const bounds = [
              [0, 0], // top-left corner
              [height, width], // bottom-right corner
            ];

            const imageUrl = canvas.toDataURL();
            L.imageOverlay(imageUrl, bounds).addTo(map);
          }
        }
      } catch (error) {
        console.error("Error loading TIFF:", error);
      }
    }
  };

  return (
    <div>
      {/* File input */}
      <input type="file" accept=".tiff,.tif" onChange={handleFileUpload} />

      {/* Leaflet map */}
      <div
        ref={mapRef}
        style={{ width: "100%", height: "500px", marginTop: "10px" }}
      ></div>
    </div>
  );
};

export default DEMMap;

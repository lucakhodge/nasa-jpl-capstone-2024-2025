import React, { useState } from "react";
import { DEMData } from "src/IPC/loadDEM";

export default function MyGeoTIff() {
  const [demData, setDemData] = useState<any>(null);

  const handleFileSelect = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (file) {
      const filePath = file.path;
      console.log(file, filePath, window.electronIPC);
      window.electronIPC.loadDEM(file).then((data: DEMData) => {
        setDemData(data);
        // Optionally, trigger rendering of the DEM data here
      });
    }
  };

  return (
    <div>
      <div>MyGeoTIff</div>
      <input type="file" onChange={handleFileSelect} />
      {demData && (
        <div>
          DEM Loaded: {demData.width}x{demData.height}
        </div>
      )}
      {/* Render the map here */}
    </div>
  );
}

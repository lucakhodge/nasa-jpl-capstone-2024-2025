import React from 'react';

interface PathAnalyticsBoxProps {
  // You might want to pass in path data here
  pathData?: {
    totalDistance?: number;
    elevationGain?: number;
    elevationLoss?: number;
    maxElevation?: number;
    minElevation?: number;
    estimatedTime?: number;
    energyConsumption?: number;
    terrainDifficulty?: string;
  };
}

export const PathAnalyticsBox: React.FC<PathAnalyticsBoxProps> = ({ 
  pathData = {} 
}) => {
  const {
    totalDistance = 0,
    elevationGain = 0,
    elevationLoss = 0,
    maxElevation = 0,
    minElevation = 0,
    estimatedTime = 0,
    energyConsumption = 0,
    terrainDifficulty = 'N/A'
  } = pathData;

  return (
    <div className="bg-white rounded-lg shadow-md p-4 h-full">
      <h2 className="text-xl font-bold mb-4 text-red-800">Path Analytics</h2>
      
      <div className="space-y-3">
        <div className="border-b pb-2">
          <h3 className="text-sm font-semibold text-gray-700">Distance</h3>
          <p className="text-lg">{totalDistance.toFixed(2)} km</p>
        </div>
        
        <div className="border-b pb-2">
          <h3 className="text-sm font-semibold text-gray-700">Elevation</h3>
          <div className="grid grid-cols-2 gap-2 mt-1">
            <div>
              <p className="text-xs text-gray-600">Gain</p>
              <p className="text-md">+{elevationGain.toFixed(1)} m</p>
            </div>
            <div>
              <p className="text-xs text-gray-600">Loss</p>
              <p className="text-md">-{elevationLoss.toFixed(1)} m</p>
            </div>
            <div>
              <p className="text-xs text-gray-600">Max</p>
              <p className="text-md">{maxElevation.toFixed(1)} m</p>
            </div>
            <div>
              <p className="text-xs text-gray-600">Min</p>
              <p className="text-md">{minElevation.toFixed(1)} m</p>
            </div>
          </div>
        </div>
        
        <div className="border-b pb-2">
          <h3 className="text-sm font-semibold text-gray-700">Estimated Travel Time</h3>
          <p className="text-lg">{estimatedTime.toFixed(1)} hours</p>
        </div>
        
        <div className="border-b pb-2">
          <h3 className="text-sm font-semibold text-gray-700">Energy Consumption</h3>
          <p className="text-lg">{energyConsumption.toFixed(1)} kWh</p>
        </div>
        
        <div>
          <h3 className="text-sm font-semibold text-gray-700">Terrain Difficulty</h3>
          <p className="text-lg">{terrainDifficulty}</p>
        </div>
      </div>
    </div>
  );
};
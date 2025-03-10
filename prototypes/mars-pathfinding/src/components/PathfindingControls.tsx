import React, { useState } from 'react';
import { runPathfinding, PathfindingResult } from '../services/pathfinding';

interface PathfindingControlsProps {
  demFile: string;
  onPathFound: (result: PathfindingResult) => void;
}

export default function PathfindingControls({ demFile, onPathFound }: PathfindingControlsProps) {
  const [startX, setStartX] = useState(100);
  const [startY, setStartY] = useState(100);
  const [endX, setEndX] = useState(200);
  const [endY, setEndY] = useState(200);
  const [maxSlope, setMaxSlope] = useState(30);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  
  const handleFindPath = async () => {
    setLoading(true);
    setError(null);
    
    try {
      const result = await runPathfinding(
        demFile,
        startX,
        startY,
        endX,
        endY,
        maxSlope
      );
      
      onPathFound(result);
    } catch (err) {
      setError(`Pathfinding failed: ${err.message}`);
      console.error('Pathfinding error:', err);
    } finally {
      setLoading(false);
    }
  };
  
  return (
    <div className="p-4 bg-gray-100 rounded">
      <h3 className="font-bold mb-3">Pathfinding Controls</h3>
      <div className="grid grid-cols-2 gap-2 mb-3">
        <div>
          <label className="block text-sm">Start X</label>
          <input 
            type="number" 
            value={startX}
            onChange={e => setStartX(Number(e.target.value))}
            className="w-full p-1 border"
          />
        </div>
        <div>
          <label className="block text-sm">Start Y</label>
          <input 
            type="number" 
            value={startY}
            onChange={e => setStartY(Number(e.target.value))}
            className="w-full p-1 border"
          />
        </div>
        <div>
          <label className="block text-sm">End X</label>
          <input 
            type="number" 
            value={endX}
            onChange={e => setEndX(Number(e.target.value))}
            className="w-full p-1 border"
          />
        </div>
        <div>
          <label className="block text-sm">End Y</label>
          <input 
            type="number" 
            value={endY}
            onChange={e => setEndY(Number(e.target.value))}
            className="w-full p-1 border"
          />
        </div>
      </div>
      
      <div className="mb-4">
        <label className="block text-sm">Max Slope: {maxSlope}°</label>
        <input 
          type="range"
          min="0"
          max="90"
          value={maxSlope}
          onChange={e => setMaxSlope(Number(e.target.value))}
          className="w-full"
        />
      </div>
      
      <button 
        onClick={handleFindPath}
        disabled={loading}
        className="w-full p-2 bg-blue-500 text-white rounded"
      >
        {loading ? 'Calculating...' : 'Find Path'}
      </button>
      
      {error && (
        <div className="mt-2 text-red-500">{error}</div>
      )}
    </div>
  );
}
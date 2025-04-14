import React, { useEffect, useState } from 'react';
import { useAppSelector } from '../store/hooks';
import { selectPath } from '../store/mapSlice';
import { selectParameters } from '../store/paramatersSlice';

interface PathMetrics {
  totalDistance: string;
  horizontalDistance: string;
  totalElevationChange: string;
  netElevationChange: string;
  maxSlope: string;
  avgSlope: string;
  energyCost: string;
  baseElevation: string;
  waypointCount: number;
}

export default function PathAnalytics() {
  const [metrics, setMetrics] = useState<PathMetrics | null>(null);
  const [loading, setLoading] = useState(false);
  const path = useAppSelector(selectPath);
  const params = useAppSelector(selectParameters);

  useEffect(() => {
    async function fetchPathAnalytics() {
      if (!path || path.length === 0) return;
      
      setLoading(true);
      try {
        // Get the start and end coordinates from the path
        const start = path[0];
        const end = path[path.length - 1];
        
        // Fetch analytics from the electron process
        const data = await window.electronIPC.getPathAnalytics(
          start.x, start.y, end.x, end.y
        );
        
        if (data) {
          setMetrics(data);
        }
      } catch (error) {
        console.error('Error fetching path analytics:', error);
      } finally {
        setLoading(false);
      }
    }
    
    fetchPathAnalytics();
  }, [path]);

  if (loading) {
    return <div className="p-4 bg-white/10 rounded-lg">Loading path analytics...</div>;
  }

  if (!metrics) {
    return <div className="p-4 bg-white/10 rounded-lg">No path analytics available</div>;
  }

  return (
    <div className="p-4 bg-white/10 rounded-lg text-black">
      <h2 className="text-xl font-orbitron mb-3">Mars Rover Path Analysis</h2>
      <div className="grid grid-cols-2 gap-2">
        <div className="col-span-2 font-bold">Metrics:</div>
        <div>Total Distance:</div>
        <div>{metrics.totalDistance} m</div>
        <div>Horizontal Distance:</div>
        <div>{metrics.horizontalDistance} m</div>
        <div>Total Elevation Change:</div>
        <div>{metrics.totalElevationChange} m</div>
        <div>Net Elevation Change:</div>
        <div>{metrics.netElevationChange} m</div>
        <div>Maximum Slope:</div>
        <div>{metrics.maxSlope}°</div>
        <div>Average Slope:</div>
        <div>{metrics.avgSlope}°</div>
        <div>Energy Cost:</div>
        <div>{metrics.energyCost}</div>
        <div>Base Elevation:</div>
        <div>{metrics.baseElevation} m</div>
        <div>Waypoints:</div>
        <div>{metrics.waypointCount}</div>
      </div>
    </div>
  );
}
import React, { useEffect, useState } from 'react';
import { PathMetrics } from '../IPC/electronIPC';

export default function PathAnalytics() {
  const [metrics, setMetrics] = useState<PathMetrics | null>(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    async function loadAnalytics() {
      try {
        // For testing, use fixed coordinates 
        // In production, you would get these from your path data
        const startX = 1254;
        const startY = 1265;
        const endX = 1340;
        const endY = 1338;
        
        const data = await window.electronIPC.getPathAnalytics(startX, startY, endX, endY);
        setMetrics(data);
      } catch (error) {
        console.error('Error loading path analytics:', error);
      } finally {
        setLoading(false);
      }
    }
    
    loadAnalytics();
  }, []);

  if (loading) {
    return <div>Loading analytics...</div>;
  }

  if (!metrics) {
    return <div>No analytics data available</div>;
  }

  return (
    <div className="bg-white/20 p-4 rounded-lg">
      <h2 className="font-bold text-lg mb-2">Path Analytics</h2>
      <div className="grid grid-cols-2 gap-2 text-sm">
        <div>Total Distance:</div>
        <div>{metrics.totalDistance} m</div>
        <div>Horizontal Distance:</div>
        <div>{metrics.horizontalDistance} m</div>
        <div>Total Elevation Change:</div>
        <div>{metrics.totalElevationChange} m</div>
        <div>Maximum Slope:</div>
        <div>{metrics.maxSlope}°</div>
        <div>Average Slope:</div>
        <div>{metrics.avgSlope}°</div>
        <div>Energy Cost:</div>
        <div>{metrics.energyCost}</div>
      </div>
    </div>
  );
}
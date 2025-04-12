import { useState, useEffect } from 'react';
import { LoadMapChunkFromPath } from '../components/LoadMapChunkFromPath';
import { MyButton } from '../components/MyButton';

interface MapPagePropsI {
  onBack: () => void;
}

export default function (props: MapPagePropsI) {
  const [pathData, setPathData] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  // Function to load metrics file
  const loadMetricsFile = async () => {
    setLoading(true);
    setError(null);
    
    try {
      console.log('Trying to find and load metrics file...');
      
      // Use a direct path for now since findMetricsFile may not be working
      const metricsFilePath = '/var/folders/0l/krbnptvd7bz827zv1f7r0p140000gn/T/path-result';
      console.log('Looking for metrics file at:', metricsFilePath);
      
      // Read the file
      const fileContent = await window.electronIPC.readFile(metricsFilePath);
      
      if (!fileContent || fileContent.trim() === '') {
        throw new Error('Metrics file is empty');
      }
      
      // Parse the JSON data
      const jsonData = JSON.parse(fileContent);
      console.log('Successfully parsed metrics data');
      
      if (jsonData && jsonData.metrics) {
        const metrics = jsonData.metrics;
        
        // Transform to the required format - keeping units consistent
        const transformedData = {
          totalDistance: metrics.totalDistance, // Keep in meters
          elevationGain: metrics.netElevationChange > 0 ? metrics.netElevationChange : 0,
          elevationLoss: metrics.netElevationChange < 0 ? Math.abs(metrics.netElevationChange) : 0,
          maxSlope: metrics.maxSlope,
          averageSlope: metrics.averageSlope,
          maxElevation: metrics.netElevationChange > 0 ? metrics.netElevationChange : 0,
          minElevation: metrics.netElevationChange < 0 ? Math.abs(metrics.netElevationChange) : 0,
          baseElevation: 0,
          waypointCount: jsonData.data ? jsonData.data.length : 0
        };
        
        setPathData(transformedData);
      } else {
        throw new Error('No metrics found in file');
      }
    } catch (error) {
      console.error('Error loading metrics:', error);
      setError(`Could not load metrics: ${error.message}`);
    } finally {
      setLoading(false);
    }
  };

  // Load metrics on component mount and periodically
  useEffect(() => {
    // Initial load
    loadMetricsFile();
    
    // Set up periodic polling
    const intervalId = setInterval(() => {
      loadMetricsFile();
    }, 5000); // Check every 5 seconds
    
    return () => clearInterval(intervalId);
  }, []);

  return (
    <div className="w-screen h-screen flex flex-col gap-4" style={{ backgroundColor: '#D1945A' }}>
      <div className="flex flex-1">
        <div className="flex-1 bg-white bg-opacity-50 m-2 rounded-lg">
          <LoadMapChunkFromPath></LoadMapChunkFromPath>
        </div>
        <div className="w-80 p-4 bg-white rounded-lg shadow-md m-2">
          <div className="flex justify-between items-center mb-4">
            <h2 className="text-lg font-semibold">Path Metrics</h2>
            {loading ? (
              <span className="text-xs text-gray-500">Updating...</span>
            ) : (
              <button 
                onClick={loadMetricsFile}
                className="text-xs bg-blue-500 hover:bg-blue-600 text-white px-2 py-1 rounded"
              >
                Refresh
              </button>
            )}
          </div>
          
          {error && (
            <div className="mb-3 p-2 bg-yellow-100 text-yellow-800 text-sm rounded">
              {error}
            </div>
          )}
          
          {loading && !pathData ? (
            <div className="flex justify-center items-center h-32">
              <p className="text-gray-500">Loading metrics...</p>
            </div>
          ) : pathData ? (
            <div className="space-y-4">
              <div className="bg-gray-50 rounded-lg p-3">
                <h3 className="font-medium text-gray-700 mb-2">Distance</h3>
                <p className="text-xl font-bold">{Math.round(pathData.totalDistance)} m</p>
              </div>
              
              <div className="bg-gray-50 rounded-lg p-3">
                <h3 className="font-medium text-gray-700 mb-2">Elevation</h3>
                <div className="grid grid-cols-2 gap-2">
                  <div>
                    <p className="text-xs text-gray-500">Gain</p>
                    <p className="font-semibold text-green-600">+{pathData.elevationGain.toFixed(1)} m</p>
                  </div>
                  <div>
                    <p className="text-xs text-gray-500">Loss</p>
                    <p className="font-semibold text-red-600">-{pathData.elevationLoss.toFixed(1)} m</p>
                  </div>
                  <div>
                    <p className="text-xs text-gray-500">Max</p>
                    <p className="font-semibold">{pathData.maxElevation.toFixed(1)} m</p>
                  </div>
                  <div>
                    <p className="text-xs text-gray-500">Min</p>
                    <p className="font-semibold">{pathData.minElevation.toFixed(1)} m</p>
                  </div>
                </div>
              </div>
              
              <div className="bg-gray-50 rounded-lg p-3">
                <h3 className="font-medium text-gray-700 mb-2">Slope</h3>
                <div className="grid grid-cols-2 gap-2">
                  <div>
                    <p className="text-xs text-gray-500">Maximum</p>
                    <p className="font-semibold">{pathData.maxSlope.toFixed(1)}°</p>
                  </div>
                  <div>
                    <p className="text-xs text-gray-500">Average</p>
                    <p className="font-semibold">{pathData.averageSlope.toFixed(1)}°</p>
                  </div>
                </div>
              </div>
              
              <div className="bg-gray-50 rounded-lg p-3">
                <h3 className="font-medium text-gray-700 mb-2">Waypoints</h3>
                <p className="text-xl font-bold">{pathData.waypointCount}</p>
              </div>
            </div>
          ) : (
            <div className="flex flex-col items-center justify-center h-32">
              <p className="text-gray-500">No metrics available</p>
              <p className="text-gray-400 text-sm mt-1">Calculate a path to see metrics</p>
            </div>
          )}
        </div>
      </div>
      <div className="mt-auto flex justify-between p-4">
        <MyButton onClick={props.onBack}>Back</MyButton>
      </div>
    </div>
  );
}
import { useState, useEffect } from 'react';
import { useAppSelector } from '../store/hooks';
import { LoadMapChunkFromPath } from '../components/LoadMapChunkFromPath';
import { MyButton } from '../components/MyButton';
import { selectPathNotFound } from '../store/pathSlice';

interface MapPagePropsI {
  onBack: () => void;
}

export default function MapPage(props: MapPagePropsI) {
  const [pathData, setPathData] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  
  // Get path not found status from Redux store
  const pathNotFound = useAppSelector(selectPathNotFound);

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
    <div 
      className="w-screen h-screen flex flex-col gap-4" 
      style={{ 
        background: '#000',
        backgroundImage: `
          radial-gradient(white, rgba(255,255,255,.2) 2px, transparent 3px),
          radial-gradient(white, rgba(255,255,255,.15) 1px, transparent 2px),
          radial-gradient(white, rgba(255,255,255,.1) 1px, transparent 1px)
        `,
        backgroundSize: '550px 550px, 350px 350px, 250px 250px',
        backgroundPosition: '0 0, 40px 60px, 130px 270px'
      }}
    >
      <div className="flex flex-1">
        <div className="flex-1 bg-black bg-opacity-30 m-2 rounded-lg border border-gray-700">
          {pathNotFound ? (
            <div className="flex flex-col items-center justify-center h-full p-6 text-center">
              <div className="text-yellow-300 text-4xl mb-4">⚠️</div>
              <h3 className="text-lg font-semibold text-white mb-2">No Path Found</h3>
              <p className="text-gray-300 mb-4">
                The algorithm couldn't find a valid path between your selected points.
              </p>
              <div className="bg-gray-800 p-4 rounded-lg border border-gray-700 w-full max-w-md">
                <h4 className="font-medium text-blue-300 mb-2">Try these solutions:</h4>
                <ul className="text-sm text-gray-300 text-left list-disc pl-5 space-y-1">
                  <li>Increase the slope tolerance</li>
                  <li>Select points that are closer together</li>
                  <li>Choose points with less extreme elevation differences</li>
                  <li>Verify that a path is physically possible in the selected region</li>
                </ul>
              </div>
              <MyButton 
                className="mt-6"
                onClick={props.onBack}
              >
                Adjust Parameters
              </MyButton>
            </div>
          ) : (
            <LoadMapChunkFromPath />
          )}
        </div>
        <div className="w-80 p-4 bg-gray-900 text-white rounded-lg shadow-lg m-2 border border-gray-700">
          <div className="flex justify-between items-center mb-4">
            <h2 className="text-lg font-semibold text-blue-300">Path Metrics</h2>
            {loading ? (
              <span className="text-xs text-gray-400">Updating...</span>
            ) : (
              <button 
                onClick={loadMetricsFile}
                className="text-xs bg-blue-600 hover:bg-blue-700 text-white px-2 py-1 rounded"
              >
                Refresh
              </button>
            )}
          </div>
          
          {error && (
            <div className="mb-3 p-2 bg-yellow-900 text-yellow-200 text-sm rounded">
              {error}
            </div>
          )}
          
          {loading && !pathData ? (
            <div className="flex justify-center items-center h-32">
              <p className="text-gray-400">Loading metrics...</p>
            </div>
          ) : pathData ? (
            <div className="space-y-4">
              <div className="bg-gray-800 rounded-lg p-3 border border-gray-700">
                <h3 className="font-medium text-gray-300 mb-2">Distance</h3>
                <p className="text-xl font-bold text-blue-300">{Math.round(pathData.totalDistance)} m</p>
              </div>
              
              <div className="bg-gray-800 rounded-lg p-3 border border-gray-700">
                <h3 className="font-medium text-gray-300 mb-2">Elevation</h3>
                <div className="grid grid-cols-2 gap-2">
                  <div>
                    <p className="text-xs text-gray-400">Gain</p>
                    <p className="font-semibold text-green-400">+{pathData.elevationGain.toFixed(1)} m</p>
                  </div>
                  <div>
                    <p className="text-xs text-gray-400">Loss</p>
                    <p className="font-semibold text-red-400">-{pathData.elevationLoss.toFixed(1)} m</p>
                  </div>
                  <div>
                    <p className="text-xs text-gray-400">Max</p>
                    <p className="font-semibold text-cyan-300">{pathData.maxElevation.toFixed(1)} m</p>
                  </div>
                  <div>
                    <p className="text-xs text-gray-400">Min</p>
                    <p className="font-semibold text-cyan-300">{pathData.minElevation.toFixed(1)} m</p>
                  </div>
                </div>
              </div>
              
              <div className="bg-gray-800 rounded-lg p-3 border border-gray-700">
                <h3 className="font-medium text-gray-300 mb-2">Slope</h3>
                <div className="grid grid-cols-2 gap-2">
                  <div>
                    <p className="text-xs text-gray-400">Maximum</p>
                    <p className="font-semibold text-yellow-300">{pathData.maxSlope.toFixed(1)}°</p>
                  </div>
                  <div>
                    <p className="text-xs text-gray-400">Average</p>
                    <p className="font-semibold text-yellow-300">{pathData.averageSlope.toFixed(1)}°</p>
                  </div>
                </div>
              </div>
              
              <div className="bg-gray-800 rounded-lg p-3 border border-gray-700">
                <h3 className="font-medium text-gray-300 mb-2">Waypoints</h3>
                <p className="text-xl font-bold text-purple-300">{pathData.waypointCount}</p>
              </div>
            </div>
          ) : pathNotFound ? (
            <div className="flex flex-col items-center justify-center h-32">
              <p className="text-yellow-300 text-sm mb-2">No path could be found</p>
              <p className="text-gray-500 text-xs text-center">Try adjusting your parameters and calculate again</p>
            </div>
          ) : (
            <div className="flex flex-col items-center justify-center h-32">
              <p className="text-gray-400">No metrics available</p>
              <p className="text-gray-500 text-sm mt-1">Calculate a path to see metrics</p>
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
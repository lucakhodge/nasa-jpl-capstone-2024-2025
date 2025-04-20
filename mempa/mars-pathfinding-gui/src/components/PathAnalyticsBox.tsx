import React from 'react';
import { Metrics } from '../IPC/electronIPC';

interface PathAnalyticsBoxPropsI {
  metrics: Metrics,
  // metrics: {
  //   totalDistance: number;
  //   elevationGain: number;
  //   elevationLoss: number;
  //   maxElevation: number;
  //   minElevation: number;
  //   maxSlope: number;     // Add maximum slope
  //   averageSlope: number; // Add average slope
  //   baseElevation: number; // Add base elevation
  // };
}

export const PathAnalyticsBox
  = (props: PathAnalyticsBoxPropsI) => {

    return (
      <div className="w-80 p-4 bg-gray-900 text-white rounded-lg shadow-lg m-2 border border-gray-700">
        <div className="flex justify-between items-center mb-4">
          <h2 className="text-lg font-semibold text-blue-300">Path Metrics</h2>
        </div>
        <div className="space-y-4">
          <div className="bg-gray-800 rounded-lg p-3 border border-gray-700">
            <h3 className="font-medium text-gray-300 mb-2">Staight-line Distance</h3>
            <p className="text-xl font-bold text-blue-300">{Math.round(props.metrics.asTheCrowFlysDistance)} m</p>
          </div>

          <div className="bg-gray-800 rounded-lg p-3 border border-gray-700">
            <h3 className="font-medium text-gray-300 mb-2">Odometer Distance</h3>
            <p className="text-xl font-bold text-blue-300">{Math.round(props.metrics.totalDistance)} m</p>
          </div>

          <div className="bg-gray-800 rounded-lg p-3 border border-gray-700">
            <h3 className="font-medium text-gray-300 mb-2">Net Elevation</h3>
            <div className="grid grid-cols-2 gap-2">
              <div>
                <p className="text-xs text-gray-400">Gain</p>
                <p className="font-semibold text-green-400">+{props.metrics.elevationGain.toFixed(1)} m</p>
              </div>
              <div>
                <p className="text-xs text-gray-400">Loss</p>
                <p className="font-semibold text-red-400">-{props.metrics.elevationLoss.toFixed(1)} m</p>
              </div>
            </div>
          </div>

          <div className="bg-gray-800 rounded-lg p-3 border border-gray-700">
            <h3 className="font-medium text-gray-300 mb-2">Slope</h3>
            <div className="grid grid-cols-2 gap-2">
              <div>
                <p className="text-xs text-gray-400">Maximum</p>
                <p className="font-semibold text-yellow-300">{props.metrics.maxSlope.toFixed(1)}°</p>
              </div>
              <div>
                <p className="text-xs text-gray-400">Average</p>
                <p className="font-semibold text-yellow-300">{props.metrics.averageSlope.toFixed(1)}°</p>
              </div>
            </div>
          </div>

        </div>

      </div>
    );
  };

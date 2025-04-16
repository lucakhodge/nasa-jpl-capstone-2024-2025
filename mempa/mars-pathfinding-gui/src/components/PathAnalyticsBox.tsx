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
      false
        ?
        <div className="bg-white rounded-lg shadow-md p-4 h-full">
          <h2 className="text-xl font-bold mb-4 text-red-800">Path Analytics</h2>

          <div className="space-y-3">
            <div className="border-b pb-2">
              <h3 className="text-sm font-semibold text-gray-700">Distance</h3>
              <p className="text-lg">{props.metrics.totalDistance.toFixed(2)} km</p>
            </div>

            <div className="border-b pb-2">
              <h3 className="text-sm font-semibold text-gray-700">Elevation</h3>
              <div className="grid grid-cols-2 gap-2 mt-1">
                <div>
                  <p className="text-xs text-gray-600">Gain</p>
                  <p className="text-md">+{props.metrics.elevationGain.toFixed(1)} m</p>
                </div>
                <div>
                  <p className="text-xs text-gray-600">Loss</p>
                  <p className="text-md">-{props.metrics.elevationLoss.toFixed(1)} m</p>
                </div>
                <div>
                  <p className="text-xs text-gray-600">Max</p>
                  <p className="text-md">{props.metrics.maxElevation.toFixed(1)} m</p>
                </div>
                <div>
                  <p className="text-xs text-gray-600">Min</p>
                  <p className="text-md">{props.metrics.minElevation.toFixed(1)} m</p>
                </div>
                <div>
                  <p className="text-xs text-gray-600">Base</p>
                  <p className="text-md">{props.metrics.baseElevation.toFixed(1)} m</p>
                </div>
              </div>
            </div>

            <div className="border-b pb-2">
              <h3 className="text-sm font-semibold text-gray-700">Slope</h3>
              <div className="grid grid-cols-2 gap-2 mt-1">
                <div>
                  <p className="text-xs text-gray-600">Maximum</p>
                  <p className="text-md">{props.metrics.maxSlope.toFixed(1)}°</p>
                </div>
                <div>
                  <p className="text-xs text-gray-600">Average</p>
                  <p className="text-md">{props.metrics.averageSlope.toFixed(1)}°</p>
                </div>
              </div>
            </div>

          </div>
        </div>
        :
        <div className="w-80 p-4 bg-gray-900 text-white rounded-lg shadow-lg m-2 border border-gray-700">
          <div className="flex justify-between items-center mb-4">
            <h2 className="text-lg font-semibold text-blue-300">Path Metrics</h2>
          </div>
          <div className="space-y-4">
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
                <div>
                  <p className="text-xs text-gray-400">Max</p>
                  <p className="font-semibold text-cyan-300">{props.metrics.maxElevation.toFixed(1)} m</p>
                </div>
                <div>
                  <p className="text-xs text-gray-400">Min</p>
                  <p className="font-semibold text-cyan-300">{props.metrics.minElevation.toFixed(1)} m</p>
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

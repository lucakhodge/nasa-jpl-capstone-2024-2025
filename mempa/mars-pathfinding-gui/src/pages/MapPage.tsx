import { useState, useEffect, useRef } from 'react';
import { useAppSelector, useAppDispatch } from '../store/hooks';
import { LoadMapChunkFromPath } from '../components/LoadMapChunkFromPath';
import { MyButton } from '../components/MyButton';
// import { selectPathNotFound, selectPath, selectPathLoading, clearPath, setPathLoading } from '../store/pathSlice';
import { selectLoadState, selectMetrics, selectPath } from '../store/mapSlice'
import { PathAnalyticsBox } from '../components/PathAnalyticsBox';

interface MapPagePropsI {
  onBack: () => void;
}

export default function MapPage(props: MapPagePropsI) {
  const path = useAppSelector(selectPath);
  const metrics = useAppSelector(selectMetrics);
  const loadState = useAppSelector(selectLoadState);
  const pathNotFound = path === null;

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
          {(loadState === 'error' || loadState === 'idle') && (
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
                // className="mt-6"
                onClick={props.onBack}
              >
                Adjust Parameters
              </MyButton>
            </div>
          )}
          {loadState === 'loading' && (
            <div className="flex flex-col items-center justify-center h-full">
              <div className="text-blue-300 mb-3">
                <svg className="animate-spin h-10 w-10" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
                  <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="4"></circle>
                  <path className="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
                </svg>
              </div>
              <p className="text-white font-medium">Calculating path...</p>
            </div>
          )}
          {loadState === 'loaded' && (
            <div className="w-full h-full p-4">
              <LoadMapChunkFromPath extendedBoundary={true} edgeSmoothing={20} />
            </div>
          )}

        </div>
        {pathNotFound ? <div>Not found</div> :
          <PathAnalyticsBox metrics={metrics} ></PathAnalyticsBox>
        }
      </div>
      <div className="mt-auto flex justify-between p-4">
        <MyButton onClick={props.onBack}>Back</MyButton>
      </div>
    </div>
  );
}
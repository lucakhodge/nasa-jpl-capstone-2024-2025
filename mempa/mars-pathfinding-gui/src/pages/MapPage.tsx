import { useState, useEffect, useRef } from 'react';
import { useAppSelector, useAppDispatch } from '../store/hooks';
import { LoadMapChunkFromPath } from '../components/LoadMapChunkFromPath';
import { MyButton } from '../components/MyButton';
// import { selectPathNotFound, selectPath, selectPathLoading, clearPath, setPathLoading } from '../store/pathSlice';
import { selectLoadState, selectMetrics, selectPath } from '../store/mapSlice'
import { PathAnalyticsBox } from '../components/PathAnalyticsBox';
import { CalculatingSpinner } from '../components/CalculatingSpinner';
import { PathError } from '../components/PathError';

interface MapPagePropsI {
  onBack: () => void;
}

export default function MapPage(props: MapPagePropsI) {
  const path = useAppSelector(selectPath);
  const metrics = useAppSelector(selectMetrics);
  const loadState = useAppSelector(selectLoadState);

  return (
    <div
      className="w-screen h-screen flex flex-col gap-4 overflow-scroll"
    >
      <div className="w-full flex flex-1">
        {(loadState === 'error' || loadState === 'idle') && (
          <PathError onBack={props.onBack} />
        )}
        {loadState === 'loading' && (
          <CalculatingSpinner />
        )}
        {loadState === 'loaded' && (
          <>
            <div className="flex-1 bg-black bg-opacity-30 m-2 rounded-lg border border-gray-700">
              <div className="w-full h-full p-4">
                <LoadMapChunkFromPath />
              </div>
            </div>
            <PathAnalyticsBox metrics={metrics} ></PathAnalyticsBox>
          </>
        )}
      </div>
      <div className="mt-auto flex justify-between p-4">
        <MyButton onClick={props.onBack}>Back</MyButton>
      </div>
    </div>
  );
}

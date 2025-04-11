import { LoadMapChunkFromPath } from '../components/LoadMapChunkFromPath';
import { MyButton } from '../components/MyButton';
import { PathAnalyticsBox } from '../components/PathAnalyticsBox';

interface MapPagePropsI {
  onBack: () => void;
}

export default function (props: MapPagePropsI) {
  // Hardcoded sample metrics
  const samplePathData = {
    totalDistance: 6.13,  // in km
    elevationGain: 3025.0,
    elevationLoss: 0,
    maxElevation: 3025.0,
    minElevation: 0,
    maxSlope: 89.1,
    averageSlope: 41.8,
    baseElevation: 0,
    waypointCount: 201
  };

  return (
    <div className="w-screen h-screen flex flex-col gap-4" style={{ backgroundColor: '#D1945A' }} >
      <div className="flex flex-1">
        <div className="flex-1">
          <LoadMapChunkFromPath></LoadMapChunkFromPath>
        </div>
        <div className="w-80 p-4">
          <PathAnalyticsBox pathData={samplePathData} />
        </div>
      </div>
      <div className="mt-auto flex justify-between p-4">
        <MyButton onClick={props.onBack}>Back</MyButton>
      </div>
    </div>
  )
}
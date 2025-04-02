import { LoadMapChunkFromPath } from '../components/LoadMapChunkFromPath';
import { MyButton } from '../components/MyButton';
import { PathAnalyticsBox } from '../components/PathAnalyticsBox'; // Add this import

interface MapPagePropsI {
  onBack: () => void;
}

export default function (props: MapPagePropsI) {
  return (
    <div className="w-screen h-screen flex flex-col gap-4" style={{ backgroundColor: '#D1945A' }} >
      <div className="flex flex-1">
        <div className="flex-1">
          <LoadMapChunkFromPath></LoadMapChunkFromPath>
        </div>
        <div className="w-80 p-4">
          <PathAnalyticsBox />
        </div>
      </div>
      <div className="mt-auto flex justify-between p-4">
        <MyButton onClick={props.onBack}>Back</MyButton>
      </div>
    </div>
  )
}
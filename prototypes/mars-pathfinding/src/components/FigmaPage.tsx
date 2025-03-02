import { useAppDispatch, useAppSelector } from '../store/hooks';
import { selectEndCoordinate, selectSlope, selectStartCoordinate, setEndCoordinate, setSlope, setStartCoordinate } from '../store/paramatersSlice';
import FileStatus from './FileStatus';
import GeneratePathButton from './GeneratePathButton';
import LoadFileButton from './LoadFileButton';
import { LoadMapChunkFromPath } from './LoadMapChunkFromPath';
import { MyNumberInput } from './MyNumberInput';



export default function FigmaPage(props: {}) {

  const dispatch = useAppDispatch();

  const startCoordinate = useAppSelector(selectStartCoordinate);
  const endCoordinate = useAppSelector(selectEndCoordinate);
  const slope = useAppSelector(selectSlope);

  return (
    <div className="grid grid-cols-[1fr_2fr] w-full h-full border-dashed bg-blue-200 absolute">
      <div className='flex flex-col gap-1 bg-gray-600 w-full h-full, p-5'>
        <div className='text-center text-[20px] text-red-500 font-bold'>Mempa</div>
        <div className='flex flex-row'>
          <div className='whitespace-nowrap'>Start Coordinate: (</div>
          <MyNumberInput value={startCoordinate.x} onChange={(val) => {
            dispatch(setStartCoordinate({ x: val, y: startCoordinate.y }))
          }}></MyNumberInput>
          <div>,</div>
          <MyNumberInput value={startCoordinate.y} onChange={(val) => {
            dispatch(setStartCoordinate({ x: startCoordinate.x, y: val }))
          }}></MyNumberInput>
          <div>)</div>
        </div>
        <div className='flex flex-row'>
          <div className='whitespace-nowrap'>End Coordinate: (</div>
          <MyNumberInput value={endCoordinate.x} onChange={(val) => {
            dispatch(setEndCoordinate({ x: val, y: endCoordinate.y }))
          }}></MyNumberInput>
          <div>,</div>
          <MyNumberInput value={endCoordinate.y} onChange={(val) => {
            dispatch(setEndCoordinate({ x: endCoordinate.x, y: val }))
          }}></MyNumberInput>
          <div>)</div>
        </div>
        <div>Start Area</div>
        <div>End Area</div>
        <div>Number of iterations</div>
        <div className='flex flex-row'>
          <div className='mr-2'>{"Slope tolerance: "}</div>
          <MyNumberInput value={slope} onChange={(val) => {
            dispatch(setSlope(val))
          }}></MyNumberInput>
        </div>
        <FileStatus />
        <div className='flex-1' />
        <div className='grid grid-cols-2 gap-5'>
          <LoadFileButton />
          <GeneratePathButton></GeneratePathButton>
        </div>
      </div>
      <div style={{ width: "100%", height: "100%" }} className='bg-slate-500'>
        <LoadMapChunkFromPath></LoadMapChunkFromPath>
      </div>
    </div>
  )
}




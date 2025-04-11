import { useAppDispatch, useAppSelector } from "../store/hooks";
import { MyButton } from "../components/MyButton";
import { selectEndCoordinate, selectRadius, selectStartCoordinate, setEndCoordinate, setRadius, setSlope, setStartCoordinate } from "../store/paramatersSlice";
import { MyNumberInput } from "../components/MyNumberInput";
import LoadFileButton from "../components/LoadFileButton";
import GeneratePathButton from "../components/GeneratePathButton";
import { selectDemInfo } from "../store/demSlice";
import FileStatus from "../components/FileStatus";
import { selectSlope } from "../store/paramatersSlice";

interface InputPagePropsI {
  onNext: () => void;
  onBack: () => void;
}

export default function (props: InputPagePropsI) {

  const dispatch = useAppDispatch();
  const startCoordinate = useAppSelector(selectStartCoordinate);
  const endCoordinate = useAppSelector(selectEndCoordinate);
  const slope = useAppSelector(selectSlope);
  const radius = useAppSelector(selectRadius);

  const isFileLoaded = useAppSelector(selectDemInfo) !== null;

  return (
    <div className="w-screen h-screen flex flex-col gap-4 p-4" style={{ backgroundColor: '#D1945A' }}>
      <div className="font-orbitron text-center">Enter input parameters</div>

      <div>
        <LoadFileButton />
      </div>
      <FileStatus></FileStatus>

      <div className='flex flex-row text-black'>
        <div className='whitespace-nowrap'>Start Coordinate: (</div>
        <MyNumberInput disabled={!isFileLoaded} value={startCoordinate.x} onChange={(val) => {
          dispatch(setStartCoordinate({ x: val, y: startCoordinate.y }))
        }}></MyNumberInput>
        <div>,</div>
        <MyNumberInput disabled={!isFileLoaded} value={startCoordinate.y} onChange={(val) => {
          dispatch(setStartCoordinate({ x: startCoordinate.x, y: val }))
        }}></MyNumberInput>
        <div>)</div>
      </div>

      <div className='flex flex-row text-black'>
        <div className='whitespace-nowrap'>End Coordinate: (</div>
        <MyNumberInput disabled={!isFileLoaded} value={endCoordinate.x} onChange={(val) => {
          dispatch(setEndCoordinate({ x: val, y: endCoordinate.y }))
        }}></MyNumberInput>
        <div>,</div>
        <MyNumberInput disabled={!isFileLoaded} value={endCoordinate.y} onChange={(val) => {
          dispatch(setEndCoordinate({ x: endCoordinate.x, y: val }))
        }}></MyNumberInput>
        <div>)</div>
      </div>

      <div className='flex flex-row text-black'>
        <div className='mr-2'>{"Slope tolerance: "}</div>
        <MyNumberInput disabled={!isFileLoaded} value={slope} onChange={(val) => {
          dispatch(setSlope(val))
        }}></MyNumberInput>
      </div>
      <div className='flex flex-row text-black'>
        <div className='mr-2'>{"Vision radius: "}</div>
        <MyNumberInput disabled={!isFileLoaded} value={radius} onChange={(val) => {
          dispatch(setRadius(val))
        }}></MyNumberInput>
      </div>
      <div className="mt-auto flex justify-between">
        <MyButton onClick={props.onBack}>Back</MyButton>
        <GeneratePathButton onClick={props.onNext}>Next</GeneratePathButton>
      </div>
    </div>
  )
}


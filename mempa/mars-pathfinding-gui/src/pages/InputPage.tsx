import { useAppDispatch, useAppSelector } from "../store/hooks";
import { MyButton } from "../components/MyButton";
import { selectEndCoordinate, selectSlope, selectStartCoordinate, setEndCoordinate, setSlope, setStartCoordinate } from "../store/paramatersSlice";
import { MyNumberInput } from "../components/MyNumberInput";
import LoadFileButton from "../components/LoadFileButton";
import GeneratePathButton from "../components/GeneratePathButton";

interface InputPagePropsI {
  onNext: () => void;
  onBack: () => void;
}

export default function (props: InputPagePropsI) {

  const dispatch = useAppDispatch();
  const startCoordinate = useAppSelector(selectStartCoordinate);
  const endCoordinate = useAppSelector(selectEndCoordinate);
  const slope = useAppSelector(selectSlope);

  return (
    <div className="w-screen h-screen flex flex-col gap-4 p-4">
      <div>
        <LoadFileButton />
      </div>

      <div className='flex flex-row text-black'>
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

      <div className='flex flex-row text-black'>
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

      <div className='flex flex-row text-black'>
        <div className='mr-2'>{"Slope tolerance: "}</div>
        <MyNumberInput value={slope} onChange={(val) => {
          dispatch(setSlope(val))
        }}></MyNumberInput>
      </div>
      <div className="mt-auto flex justify-between">
        <MyButton onClick={props.onBack}>Back</MyButton>
        <GeneratePathButton onClick={props.onNext}>Next</GeneratePathButton>
      </div>
    </div>
  )
}


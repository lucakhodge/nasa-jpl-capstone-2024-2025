import { useEffect, useRef } from "react";
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

export default function InputPage(props: InputPagePropsI) {
  const dispatch = useAppDispatch();
  const startCoordinate = useAppSelector(selectStartCoordinate);
  const endCoordinate = useAppSelector(selectEndCoordinate);
  const slope = useAppSelector(selectSlope);
  const radius = useAppSelector(selectRadius);

  const isFileLoaded = useAppSelector(selectDemInfo) !== null;

  // const inputRefs = useRef<(HTMLInputElement | null)[]>([]);
  // const focusInput = (index: number) => {
  //   const input = inputRefs.current[index];
  //   if (input) input.focus();
  // };

  const handleStartCoordinateChange = (val: number, coord: 'x' | 'y') => {
    dispatch(setStartCoordinate({
      x: coord === 'x' ? val : startCoordinate.x,
      y: coord === 'y' ? val : startCoordinate.y
    }));
  };

  const handleEndCoordinateChange = (val: number, coord: 'x' | 'y') => {
    dispatch(setEndCoordinate({
      x: coord === 'x' ? val : endCoordinate.x,
      y: coord === 'y' ? val : endCoordinate.y
    }));
  };

  const handleSlopeChange = (val: number) => {
    dispatch(setSlope(val));
  };

  return (
    <div
      className="w-screen h-screen flex flex-col gap-4 p-4 overflow-scroll"
    >
      <div className="font-orbitron text-center text-white"></div>

      <div>
        <LoadFileButton />
      </div>
      <FileStatus />

      <div className="bg-gray-900 rounded-lg p-4 border border-gray-700">
        <h2 className="text-lg font-semibold text-blue-300 mb-4">Input Parameters</h2>

        <div className="bg-gray-800 rounded-lg p-3 border border-gray-700 mb-3">
          <h3 className="font-medium text-gray-300 mb-2">Pixel Start Coordinate</h3>
          <div className='flex flex-row items-center'>
            <div className='text-white mr-2'>(</div>
            <MyNumberInput
              disabled={!isFileLoaded}
              value={startCoordinate.x}
              onChange={(val) => handleStartCoordinateChange(val, 'x')}
            />
            <div className='text-white mx-2'>,</div>
            <MyNumberInput
              disabled={!isFileLoaded}
              value={startCoordinate.y}
              onChange={(val) => handleStartCoordinateChange(val, 'y')}
            />
            <div className='text-white ml-2'>)</div>
          </div>
        </div>

        <div className="bg-gray-800 rounded-lg p-3 border border-gray-700 mb-3">
          <h3 className="font-medium text-gray-300 mb-2">Pixel End Coordinate</h3>
          <div className='flex flex-row items-center'>
            <div className='text-white mr-2'>(</div>
            <MyNumberInput
              disabled={!isFileLoaded}
              value={endCoordinate.x}
              onChange={(val) => handleEndCoordinateChange(val, 'x')}
            />
            <div className='text-white mx-2'>,</div>
            <MyNumberInput
              disabled={!isFileLoaded}
              value={endCoordinate.y}
              onChange={(val) => handleEndCoordinateChange(val, 'y')}
            />
            <div className='text-white ml-2'>)</div>
          </div>
        </div>

        <div className="bg-gray-800 rounded-lg p-3 border border-gray-700 mb-3">
          <h3 className="font-medium text-gray-300 mb-2">Slope Tolerance</h3>
          <div className='flex flex-row items-center'>
            <MyNumberInput
              disabled={!isFileLoaded}
              value={slope}
              onChange={(val) => handleSlopeChange(val)}
            />
            <div className='text-white ml-2'>degrees</div>
          </div>
        </div>
        <div className="bg-gray-800 rounded-lg p-3 border border-gray-700">
          <h3 className="font-medium text-gray-300 mb-2">Vision Radius</h3>
          <div className='flex flex-row items-center'>
            <MyNumberInput
              disabled={!isFileLoaded}
              value={radius}
              onChange={(val) => {
                dispatch(setRadius(val))
              }}
            />
            <div className='text-white ml-2'>meters</div>
          </div>
        </div>
      </div>

      <div className="mt-auto flex justify-between p-4">
        <MyButton onClick={props.onBack}>Back</MyButton>
        <GeneratePathButton onClick={props.onNext}>Next</GeneratePathButton>
      </div>
    </div >
  );
}

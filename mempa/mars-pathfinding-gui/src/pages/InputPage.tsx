import { useEffect } from "react";
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
      className="w-screen h-screen flex flex-col gap-4 p-4"
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
      <div className="font-orbitron text-center text-white">Enter input parameters</div>

      <div>
        <LoadFileButton />
      </div>
      <FileStatus />

      <div className="bg-white bg-opacity-90 p-4 rounded-lg shadow-lg">
        <div className='flex flex-row text-black mb-3'>
          <div className='whitespace-nowrap'>Start Coordinate: (</div>
          <MyNumberInput
            disabled={!isFileLoaded}
            value={startCoordinate.x}
            onChange={(val) => handleStartCoordinateChange(val, 'x')}
          />
          <div>,</div>
          <MyNumberInput
            disabled={!isFileLoaded}
            value={startCoordinate.y}
            onChange={(val) => handleStartCoordinateChange(val, 'y')}
          />
          <div>)</div>
        </div>

        <div className='flex flex-row text-black mb-3'>
          <div className='whitespace-nowrap'>End Coordinate: (</div>
          <MyNumberInput
            disabled={!isFileLoaded}
            value={endCoordinate.x}
            onChange={(val) => handleEndCoordinateChange(val, 'x')}
          />
          <div>,</div>
          <MyNumberInput
            disabled={!isFileLoaded}
            value={endCoordinate.y}
            onChange={(val) => handleEndCoordinateChange(val, 'y')}
          />
          <div>)</div>
        </div>

        <div className='flex flex-row text-black mb-3'>
          <div className='mr-2'>{"Slope tolerance: "}</div>
          <MyNumberInput
            disabled={!isFileLoaded}
            value={slope}
            onChange={(val) => handleSlopeChange(val)}
          />
        </div>

        <div className='flex flex-row text-black mb-3'>
          <div className='mr-2'>{"Vision radius: "}</div>
          <MyNumberInput disabled={!isFileLoaded} value={radius} onChange={(val) => {
            dispatch(setRadius(val))
          }}></MyNumberInput>
        </div>
      </div>

      <div className="mt-auto flex justify-between">
        <MyButton onClick={props.onBack}>Back</MyButton>
        <GeneratePathButton onClick={props.onNext}>Next</GeneratePathButton>
      </div>
    </div >
  );
}

import { useState } from 'react';
import { MyButton } from './MyButton'
import { MyNumberInput } from './MyNumberInput'
import LoadFileButton from './LoadFileButton';
import { useAppDispatch, useAppSelector } from '../store/hooks';
import { selectEndCoordinate, selectSlope, selectStartCoordinate, setEndCoordinate, setSlope, setStartCoordinate } from '../store/paramatersSlice';
import Map3d from './Map3d';
import FileStatus from './FileStatus';
import RunCpp from './RunCpp';
import nasaLogo from './images/nasa-logo-png-nasa-logo.png';

export const FigmaPage = (props: {}) => {
  const dispatch = useAppDispatch();

  const startCoordinate = useAppSelector(selectStartCoordinate);
  const endCoordinate = useAppSelector(selectEndCoordinate);
  const slope = useAppSelector(selectSlope);

  const [chunk, setChunk] = useState(null);

  const handleRetrieveChunk = () => {
    const chunk = window.electronIPC.getChunk({
      coordinate: {
        x: startCoordinate.x,
        y: startCoordinate.y,
      },
      size: {
        width: endCoordinate.x - startCoordinate.x,
        height: endCoordinate.y - startCoordinate.y,
      },
      slopeTolerance: slope, // Add the slope parameter here
    });
    setChunk(chunk);
  };

  return (
    <div className="grid grid-cols-[1fr_2fr] w-screen h-screen fixed inset-0 overflow-hidden">
      <div className='flex flex-col gap-1 w-full h-full p-5' style={{ backgroundColor: '#D1945A' }}>
        <div className='flex items-center justify-center gap-4 mb-2'>
          <img 
            src={nasaLogo} 
            alt="NASA Logo" 
            className='h-12 w-auto object-contain'
          />
          <div className='text-center text-[20px] text-red-600 font-bold font-orbitron'>
            MEMPA
          </div>
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
        <div className='text-black'>Start Area: </div>
        <div className='text-black'>End Area: </div>
        <div className='text-black'>Number of iterations: </div>
        <div className='flex flex-row text-black'>
          <div className='mr-2'>{"Slope tolerance: "}</div>
          <MyNumberInput value={slope} onChange={(val) => {
            dispatch(setSlope(val))
          }}></MyNumberInput>
        </div>
        <FileStatus />
        <RunCpp />
        <div className='flex-1' />
        <div className='grid grid-cols-2 gap-5'>
          <LoadFileButton />
          <MyButton onClick={handleRetrieveChunk}>Display Optimum Path</MyButton>
        </div>
      </div>
      <div className='relative w-full h-full bg-gray-700'>
        <div className='absolute inset-0'>
          <Map3d chunk={chunk} />
        </div>
      </div>
    </div>
  )
}
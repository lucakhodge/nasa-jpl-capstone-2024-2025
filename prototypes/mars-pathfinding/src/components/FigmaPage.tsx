import { useState } from 'react';
import { MyButton } from './MyButton'
import { MyNumberInput } from './MyNumberInput'
import LoadFileButton from './LoadFileButton';



export const FigmaPage = (props: {}) => {

  const [startCoordinateX, setStartCoordinateX] = useState<number | null>(null);
  const [startCoordinateY, setStartCoordinateY] = useState<number | null>(null);
  const [endCoordinateX, setEndCoordinateX] = useState<number | null>(null);
  const [endCoordinateY, setEndCoordinateY] = useState<number | null>(null);
  const [slope, setSlope] = useState<number | null>(null);
  return (
    <div className="grid grid-cols-[1fr_2fr] w-full h-full border-dashed bg-blue-200 absolute">
      <div className='flex flex-col gap-1 bg-gray-600 w-full h-full, p-5'>
        <div className='text-center text-[20px] text-red-500 font-bold'>Mempa</div>
        <div className='flex flex-row'>
          <div className='whitespace-nowrap'>Start Coordinate: (</div>
          <MyNumberInput value={startCoordinateX} onChange={(val) => {
            setStartCoordinateX(val)
          }}></MyNumberInput>
          <div>,</div>
          <MyNumberInput value={startCoordinateY} onChange={(val) => {
            setStartCoordinateY(val)
          }}></MyNumberInput>
          <div>)</div>
        </div>
        <div className='flex flex-row'>
          <div className='whitespace-nowrap'>End Coordinate: (</div>
          <MyNumberInput value={endCoordinateX} onChange={(val) => {
            setEndCoordinateX(val)
          }}></MyNumberInput>
          <div>,</div>
          <MyNumberInput value={endCoordinateY} onChange={(val) => {
            setEndCoordinateY(val)
          }}></MyNumberInput>
          <div>)</div>
        </div>
        <div>Start Area</div>
        <div>End Area</div>
        <div>Number of iterations</div>
        <div className='flex flex-row'>
          <div className='mr-2'>{"Slope tolerance: "}</div>
          <MyNumberInput value={slope} onChange={(val) => setSlope(val)}></MyNumberInput>
        </div>
        <div className='flex-1' />
        <div className='grid grid-cols-2 gap-5'>
          <LoadFileButton />
          <MyButton >Display Optimum Path</MyButton>
        </div>
      </div>
      <div style={{ width: "100%", height: "100%" }} className='bg-slate-500'>Map</div>
    </div>
  )
}




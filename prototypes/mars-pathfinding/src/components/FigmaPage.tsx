import React from 'react'



export const FigmaPage = (props: {}) => {
  return (
    <div className="grid grid-cols-[1fr_2fr] w-full h-full border-dashed bg-blue-200 absolute">
      <div className='flex flex-col bg-gray-700 w-full h-full, p-5'>
        <div className='text-center text-[20px] text-red-500 font-bold'>Mempa</div>
        <div>Start Coordinate</div>
        <div>End Coordinate</div>
        <div>Start Area</div>
        <div>End Area</div>
        <div>Number of iteractions</div>
        <div>slope tolerance</div>
        <div className='flex-1' />
        <div className='grid grid-cols-2 gap-5'>
          <MyButton onClick={() => console.log('clicked')}>Input DEM file</MyButton>
          <MyButton >Display Optimum Path</MyButton>
        </div>
      </div>
      <div style={{ width: "100%", height: "100%" }} className='bg-slate-500'>Map</div>
    </div>
  )
}


interface MyButtonPropsI {
  children?: React.ReactNode,
  onClick?: () => void,
}

const MyButton = (props: MyButtonPropsI) => {
  return (
    <button className='bg-slate-300 rounded hover:bg-slate-400 font-bold' onClick={props.onClick}>{props.children}</button>
  )
}


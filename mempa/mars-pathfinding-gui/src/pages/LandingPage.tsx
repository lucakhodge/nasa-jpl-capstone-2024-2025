import { MyButton } from "../components/MyButton";
import nasaLogo from "../images/nasa-logo-png-nasa-logo.png"

interface LandingPagePropsI {
  onNext: () => void;
}

export default function (props: LandingPagePropsI) {
  return (
    <div className="w-screen h-screen flex flex-col items-center justify-center gap-6" style={{ backgroundColor: '#D1945A' }}>
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
      <div className='max-w-md text-center p-4 bg-black/10 rounded-lg'>
        <p className='text-black font-medium mb-2'>Welcome to the Mars Elevation Model Pathfinding Algorithm Application</p>
        <p className='text-black/80 text-sm'>
          MEMPA helps plan optimal rover traversal paths across the Martian terrain by analyzing elevation data,
          calculating slopes, and minimizing energy consumption. Upload Mars DEM data, select your start and end
          coordinates, and discover the most efficient path for your mission.
        </p>
      </div>
      <MyButton onClick={props.onNext}>Get Started</MyButton>
    </div>
  )
}


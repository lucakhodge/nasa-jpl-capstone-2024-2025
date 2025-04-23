import { MyButton } from "../components/MyButton";
import nasaLogo from "../images/nasa-logo-png-nasa-logo.png"

interface LandingPagePropsI {
  onNext: () => void;
}

export default function (props: LandingPagePropsI) {
  return (
    <div
      className="w-screen h-screen flex flex-col items-center pt-[15%]"
    >
      <div
        className="flex flex-col items-center justify-center gap-6 p-6 max-w-xl w-full"
      >
        <div className='flex items-center justify-center gap-4 mb-2 w-full'>
          <img
            src={nasaLogo}
            alt="NASA Logo"
            className='h-12 w-auto object-contain'
          />
          <div className='text-center text-[20px] text-red-600 font-bold font-orbitron'>
            MEMPA
          </div>
        </div>
        <div className='w-full text-center p-4 bg-gray-800 rounded-lg border border-gray-700'>
          <p className='text-white font-medium mb-2'>Welcome to the Mars Elevation Model Pathfinding Algorithm Application</p>
          <p className='text-white/80 text-sm'>
            MEMPA helps plan optimal rover traversal paths across the Martian terrain by analyzing elevation data, adhering to
            slope tolerances while minimizing energy consumption. Upload DEM data, select your start and end
            coordinates, and discover the most efficient path for your mission.
          </p>
        </div>
        <div className="flex justify-center w-full">
          <MyButton onClick={props.onNext}>Get Started</MyButton>
        </div>
      </div>
    </div>
  )
}
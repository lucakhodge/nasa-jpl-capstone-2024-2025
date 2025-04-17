import { MyButton } from "../components/MyButton";
import nasaLogo from "../images/nasa-logo-png-nasa-logo.png"

interface LandingPagePropsI {
  onNext: () => void;
}

export default function (props: LandingPagePropsI) {
  return (
    <div 
      className="w-screen h-screen flex flex-col items-center justify-center gap-6" 
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
      <div className='max-w-md text-center p-4 bg-gray-800 rounded-lg p-3 border border-gray-700'>
        <p className='text-white font-medium mb-2'>Welcome to the Mars Elevation Model Pathfinding Algorithm Application</p>
        <p className='text-white/80 text-sm'>
          MEMPA helps plan optimal rover traversal paths across the Martian terrain by analyzing elevation data, adhering to
          slope tolerances while minimizing energy consumption. Upload DEM data, select your start and end
          coordinates, and discover the most efficient path for your mission.
        </p>
      </div>
      <MyButton onClick={props.onNext}>Get Started</MyButton>
    </div>
  )
}
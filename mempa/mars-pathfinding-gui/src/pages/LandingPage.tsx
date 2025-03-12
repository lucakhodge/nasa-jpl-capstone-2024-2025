import { MyButton } from "../components/MyButton";
import nasaLogo from "../images/nasa-logo-png-nasa-logo.png"

interface LandingPagePropsI {
  onNext: () => void;
}

export default function (props: LandingPagePropsI) {
  return (
    <div className="w-screen h-screen flex flex-col items-center justify-center gap-6">
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
      <div>
        Explain the project/app here
      </div>
      <MyButton onClick={props.onNext}>Get Started</MyButton>
    </div>
  )
}


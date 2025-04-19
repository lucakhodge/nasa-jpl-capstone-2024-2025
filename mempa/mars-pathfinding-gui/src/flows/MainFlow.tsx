import { useState } from "react"
import InputPage from "../pages/InputPage";
import LandingPage from "../pages/LandingPage";
import MapPage from "../pages/MapPage";


enum Pages {
  Landing,
  Input,
  Map,
}

export default function () {
  const [page, setPage] = useState<Pages>(Pages.Landing);
  return (
    <div className="w-screen h-screen overflow-hidden"
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
      {
        page === Pages.Landing ? <LandingPage onNext={() => setPage(Pages.Input)}></LandingPage> : <></>
      }
      {
        page === Pages.Input ? <InputPage onNext={() => setPage(Pages.Map)} onBack={() => setPage(Pages.Landing)}></InputPage> : <></>
      }
      {
        page === Pages.Map ? <MapPage onBack={() => setPage(Pages.Input)}></MapPage> : <></>
      }
    </div>
  )
}


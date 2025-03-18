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
    <>
      {
        page === Pages.Landing ? <LandingPage onNext={() => setPage(Pages.Input)}></LandingPage> : <></>
      }
      {
        page === Pages.Input ? <InputPage onNext={() => setPage(Pages.Map)} onBack={() => setPage(Pages.Landing)}></InputPage> : <></>
      }
      {
        page === Pages.Map ? <MapPage onBack={() => setPage(Pages.Input)}></MapPage> : <></>
      }
    </>
  )
}


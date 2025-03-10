import { selectDemInfo } from "../store/demSlice";
import { useAppSelector } from "../store/hooks";


export default function FileStatus() {

  const demInfo = useAppSelector(selectDemInfo);

  if (demInfo === null) {
    return <div className="text-red-500">{"No File Loaded"} </div>
  }
  else {
    return <div className="text-green-500">{"File Loaded:" + JSON.stringify(demInfo)}</div>
  }

}

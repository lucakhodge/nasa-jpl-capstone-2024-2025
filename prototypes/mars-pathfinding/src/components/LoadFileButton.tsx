import { useSelector } from "react-redux";
import { selectDemInfo } from "../store/demSlice";

export default function LoadFileButton() {
  const demInfo = useSelector(selectDemInfo)
  return (
    <div>
      <button
        onClick={() => {
          window.electronIPC.openDEM();
        }}
      >
        LoadFileButton
      </button>
      {demInfo ?
        <div className="text-green-300">{"width: " + demInfo.width + ", " + demInfo.height}</div>
        :
        <div className="text-red-300" >Not loaded</div>
      }
    </div>
  );
}

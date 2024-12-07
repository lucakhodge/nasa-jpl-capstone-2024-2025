import { selectDemInfo } from "../store/demSlice";
import { useAppSelector } from "../store/hooks";

export default function SampleRedux() {
  const demInfo = useAppSelector(selectDemInfo);

  return (
    <div>
      <div>{JSON.stringify(demInfo)}</div>
    </div>
  );
}

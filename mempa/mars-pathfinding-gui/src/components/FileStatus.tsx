import { selectDemInfo } from "../store/demSlice";
import { useAppSelector } from "../store/hooks";

export default function FileStatus() {
  const demInfo = useAppSelector(selectDemInfo);

  if (demInfo === null) {
    return (
      <div className="flex items-center gap-2 text-yellow-300 bg-gray-800 border border-gray-700 rounded-md px-3 py-2 text-sm">
        ⚠️ Please first select a file.
      </div>
    );
  }

  return (
    <div className="flex items-center gap-2 text-green-300 bg-gray-800 border border-gray-700 rounded-md px-3 py-2 text-sm">
      File loaded.
    </div>
  );
}

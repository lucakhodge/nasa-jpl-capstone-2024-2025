import { MyButton } from "./MyButton";

export default function LoadFileButton() {
  return (
    <MyButton onClick={() => {
      window.electronIPC.openDEM();
    }}
    className="text-green-500"
    >
      Input DEM file
    </MyButton>
  );
}

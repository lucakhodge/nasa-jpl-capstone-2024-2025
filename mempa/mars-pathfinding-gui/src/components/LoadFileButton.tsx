import { MyButton } from "./MyButton";

export default function LoadFileButton() {
  return (
    <MyButton onClick={() => {
      window.electronIPC.openDEM();
    }}
    >
      Input DEM file
    </MyButton>
  );
}

import { useAppSelector } from "../store/hooks";
import { MyButton } from "./MyButton";
import { selectParameters } from "../store/paramatersSlice";
import { useCallback, useState } from "react";

export default function CallAlgorithim() {
  const [result, setResult] = useState("");

  const parameters = useAppSelector(selectParameters);
  const handleClick = useCallback(async () => {
    const res = await window.electronIPC.callAlgorithim(parameters);
    setResult(res);
  }, []);
  return (
    <div><MyButton onClick={handleClick}>Call Algorithim</MyButton><div>{result}</div></div>
  )
}


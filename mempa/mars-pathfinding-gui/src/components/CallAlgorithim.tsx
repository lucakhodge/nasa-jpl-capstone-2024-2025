import { useAppDispatch, useAppSelector } from "../store/hooks";
import { MyButton } from "./MyButton";
import { selectEndCoordinate, selectParameters } from "../store/paramatersSlice";
import { useCallback, useState } from "react";

export default function CallAlgorithim() {

  const parameters = useAppSelector(selectParameters);

  const [result, setResult] = useState("");

  //TODO: paramters must be valid
  const handleClick = useCallback(async () => {
    const res = await window.electronIPC.callAlgorithim(parameters);
    setResult(res);
  }, [parameters]);

  return (
    <div>
      <MyButton onClick={handleClick}>Call Algorithim</MyButton>
      <div>{result}</div>
    </div>
  )
}


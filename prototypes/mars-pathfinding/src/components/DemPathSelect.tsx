import React, { useState } from "react";
import { setDemPath } from "../store/demSlice";
import { useAppDispatch } from "../store/hooks";

export default function DemPathSelect() {
  const dispatch = useAppDispatch();

  const [inputValue, setInputValue] = useState("");

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    setInputValue(e.target.value);
  };

  const onClick = () => {
    dispatch(setDemPath(inputValue));
  };

  return (
    <div>
      <input
        type="text"
        value={inputValue}
        onChange={handleInputChange}
        placeholder="Enter some text"
      />
      <button onClick={onClick}>Click to update</button>
    </div>
  );
}

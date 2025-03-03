import React, { useCallback, useState } from "react";

export default function RunCpp() {
  const [result, setResult] = useState<string>("");

  //   const callCpp = useCallback(
  //     async () => setResult(await window.electronIPC.callCpp("Hello from App.tsx!")),
  //     []
  //   );

  const callCpp = useCallback(
    async () => {
      try {
        if (!window.electronIPC?.callCpp) {
          throw new Error('electronIPC.callCpp is not available');
        }
        const response = await window.electronIPC.callCpp("Hello from App.tsx!");
        console.log("response: ", response);
        setResult(response);
      } catch (error) {
        console.error('Failed to call C++:', error);
        setResult('Error: Failed to call C++ function');
      }
    },
    []
  );
  return (
    <>
      <button onClick={callCpp}>RUN CPP</button>
      <div>{result}</div>
    </>
  );
}

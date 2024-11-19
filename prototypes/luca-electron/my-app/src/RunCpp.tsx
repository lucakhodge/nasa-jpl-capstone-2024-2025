import React, { useCallback, useState } from "react";

export default function RunCpp() {
  const [result, setResult] = useState<string>("");

  const callCpp = useCallback(
    async () => setResult(await electronIPC.callCpp("Hello from App.tsx!")),
    []
  );
  return (
    <>
      <button onClick={callCpp}>RUN CPP</button>
      <div>{result}</div>
    </>
  );
}

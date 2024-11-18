import { ipcRenderer, IpcRenderer } from "electron";
import { useCallback, useState } from "react";

function App() {
  const [count, setCount] = useState(0);
  const [result, setResult] = useState<string>("");
  // const runCppProgram = async () => {
  //   try {
  //     const output = await ipcRenderer.invoke("run-cpp");
  //     setResult(output); // Set the output from the C++ program
  //   } catch (error) {
  //     setResult("Error running C++ program");
  //   }
  // };

  const callCpp = useCallback(
    async () => setResult(await backend.cppProgram("Hello from App.tsx!")),
    []
  );

  return (
    <>
      <h1 className="text-3xl font-bold underline">Hello world!</h1>
      <div className="text-green-500 text-center font-bold">hi there</div>
      <div className="card">
        <button onClick={() => setCount((count) => count + 1)}>
          count is {count}
        </button>
        <input type="file"></input>
      </div>
      <button onClick={callCpp}>run cpp</button>
      <div>{result}</div>
    </>
  );
}

export default App;

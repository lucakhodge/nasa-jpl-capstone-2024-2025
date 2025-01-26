import React from "react";

export default function LoadFileButton() {
  return (
    <button
      onClick={() => {
        window.electronIPC.openDEM();
      }}
    >
      LoadFileButton
    </button>
  );
}

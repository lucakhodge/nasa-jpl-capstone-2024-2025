import React from "react";
import { selectDemPath } from "../store/demSlice";
import { useAppSelector } from "../store/hooks";

export default function DemPathDisplay() {
  const path = useAppSelector(selectDemPath);
  return <div>Here is the Dem Path: {path}</div>;
}

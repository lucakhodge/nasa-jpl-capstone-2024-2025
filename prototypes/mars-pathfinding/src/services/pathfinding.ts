import { ipcRenderer } from 'electron';

export interface PathfindingResult {
  path: Array<[number, number]>;
  metrics: {
    length: number;
    steps: number;
    maxSlope: number;
    avgSlope: number;
  };
}

export const runPathfinding = async (
  demFile: string,
  startX: number,
  startY: number,
  endX: number,
  endY: number,
  maxSlope: number
): Promise<PathfindingResult> => {
  return await ipcRenderer.invoke('run-pathfinding', {
    demFile,
    startX,
    startY,
    endX,
    endY,
    maxSlope
  });
};
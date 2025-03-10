export interface PathfindingResult {
  path: Array<[number, number]>;
  metrics: {
    length: number;
    steps: number;
    maxSlope: number;
    avgSlope: number;
  };
}

export async function runPathfinding(
  demFile: string,
  startX: number,
  startY: number,
  endX: number,
  endY: number,
  maxSlope: number
): Promise<PathfindingResult> {
  try {
    // Call the IPC handler for pathfinding
    const result = await window.electronIPC.runPathfinding({
      demFile,
      startX,
      startY,
      endX,
      endY,
      maxSlope
    });
    
    return result;
  } catch (error) {
    console.error('Error running pathfinding:', error);
    throw error;
  }
}
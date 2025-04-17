import { exec } from 'child_process';
import { app, ipcMain } from "electron";
import path from 'path';
import { promisify } from 'util';
import fs from 'fs';
import { CALL_ALGORITHIM, ON_ALGORITHIM_END, Parameters } from './electronIPC';
import { getDemFilePath } from './FilePathStore';
import { getRendererWindow } from '../main';

const execPromise = promisify(exec)

const getExecutablePath = () => {
  const basePath = app.isPackaged ? path.join(process.resourcesPath, 'bin') : path.join(__dirname, '../../bin');
  // Get executable name based on platform
  let executableName = 'example';
  // if (process.platform === 'win32') {
  //   executableName = 'example-win';
  // } else if (process.platform === 'darwin') {
  //   executableName = 'example-darwin';
  // } else if (process.platform === 'linux') {
  //   executableName = 'example-linux';
  // }

  return path.join(basePath, executableName);
};

const getFlags = (parameters: Parameters, inputPath: string, outputPath: string) => {
  let flagsStr = ""
  flagsStr += " --start-pixel " + parameters.startCoordinate.x + "," + parameters.startCoordinate.y
  flagsStr += " --end-pixel " + parameters.endCoordinate.x + "," + parameters.endCoordinate.y
  flagsStr += " --input " + inputPath
  flagsStr += " --output " + outputPath
  flagsStr += " --radius " + parameters.radius
  flagsStr += " --iterations " + 1
  flagsStr += " --slope " + parameters.slope
  flagsStr += " --json"
  return flagsStr;
}

ipcMain.on(CALL_ALGORITHIM, async (event, parameters: Parameters) => {

  console.log("in call algo handle, was passed:", parameters);
  const outputPath = path.join(app.getPath("temp"), 'path-result');

  try {
    await execPromise("rm " + outputPath);
  }
  catch {
  }

  const TIMEOUT_SEC = 100;
  const runWithTimeout = (cmd: string, timeout: number) => {
    return Promise.race([
      execPromise(cmd),
      new Promise<never>((_, reject) =>
        setTimeout(() => reject(new Error("Execution timed out")), timeout)
      ),
    ]);
  };

  try {
    const executableCall = getExecutablePath() + getFlags(parameters, getDemFilePath(), outputPath);
    console.log("EC: ", executableCall)
    const { stderr } = await runWithTimeout(executableCall, TIMEOUT_SEC * 1000);
    const { stderr } = await runWithTimeout(executableCall, TIMEOUT_SEC * 1000);
    if (stderr) {
      getRendererWindow().webContents.send(ON_ALGORITHIM_END, null);
      return;
    }
  } catch (error) {
    getRendererWindow().webContents.send(ON_ALGORITHIM_END, null);
    return;
  }

  const data: string = fs.readFileSync(outputPath, "utf-8");
  console.log("FILE data", data)

  let roverPath = JSON.parse(data).data;
  let metrics = JSON.parse(data).metrics;
  const transformedMetrics = {
    totalDistance: metrics.totalDistance,
    elevationGain: metrics.elevationGain,
    elevationLoss: metrics.elevationLoss,
    maxSlope: metrics.maxSlope,
    averageSlope: metrics.averageSlope,
    maxElevation: 0,
    minElevation: 0,
    baseElevation: 0,
    asTheCrowFlysDistance: metrics.asTheCrowFlysDistance
  };

  getRendererWindow().webContents.send(ON_ALGORITHIM_END, { path: roverPath, metrics: transformedMetrics });
  // event.sender.send(ON_ALGORITHIM_END, { path: roverPath, metrics: transformedMetrics });
});

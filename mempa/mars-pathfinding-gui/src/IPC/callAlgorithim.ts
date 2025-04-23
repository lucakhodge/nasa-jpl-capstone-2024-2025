import { ChildProcess, exec } from 'child_process';
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
  flagsStr += " --start " + parameters.startCoordinate.x + "," + parameters.startCoordinate.y
  flagsStr += " --end " + parameters.endCoordinate.x + "," + parameters.endCoordinate.y
  flagsStr += " --input " + inputPath
  flagsStr += " --output " + outputPath
  flagsStr += " --radius " + parameters.radius
  flagsStr += " --memory " + parameters.memory
  flagsStr += " --slope " + parameters.slope
  flagsStr += " --json"
  return flagsStr;
}

function execWithTimeout(cmd: string, timeoutMs: number) {
  let child!: ChildProcess;                             // will be assigned below

  const promise = new Promise<{ stdout: string; stderr: string }>((resolve, reject) => {
    child = exec(cmd, { maxBuffer: 1024 * 1024 }, (error, stdout, stderr) => {
      clearTimeout(timer);
      if (error) return reject(error);
      resolve({ stdout, stderr });
    });
  });

  const timer = setTimeout(() => {
    child.kill(process.platform === "win32" ? undefined : "SIGTERM");
    // force-kill after a grace period
    setTimeout(() => child.kill("SIGKILL"), 5_000);
  }, timeoutMs);

  return { child, timer, promise };
}
type ActiveRun = { child: ChildProcess; timer: NodeJS.Timeout; token: number };
let activeRun: ActiveRun | null = null;
let requestToken = 0;

function killActive() {
  if (!activeRun) return;
  activeRun.child.kill(process.platform === "win32" ? undefined : "SIGTERM");
  clearTimeout(activeRun.timer);
  activeRun = null;
}

ipcMain.on(CALL_ALGORITHIM, async (_event, parameters: Parameters) => {
  // cancel any older request right away 
  killActive();

  const myToken = ++requestToken;
  const outputPath = path.join(app.getPath("temp"), `path-result-${myToken}.json`);
  await fs.promises.rm(outputPath, { force: true }).catch(() => { });

  const cmd =
    getExecutablePath() + getFlags(parameters, getDemFilePath(), outputPath);

  // launch the new child and store it in the global slot 
  const run = execWithTimeout(cmd, 60_000);
  activeRun = { ...run, token: myToken };

  try {
    const { stderr } = await run.promise;

    // if another request arrived while we were running, bail out quietly
    if (myToken !== requestToken) return;

    if (stderr) throw new Error(stderr);

    const data = await fs.promises.readFile(outputPath, "utf-8");
    const { data: roverPath, metrics } = JSON.parse(data);

    const transformedMetrics = {
      totalDistance: metrics.totalDistance,
      elevationGain: metrics.elevationGain,
      elevationLoss: metrics.elevationLoss,
      maxSlope: metrics.maxSlope,
      averageSlope: metrics.averageSlope,
      maxElevation: 0,
      minElevation: 0,
      baseElevation: 0,
      asTheCrowFlysDistance: metrics.asTheCrowFlysDistance,
    };

    getRendererWindow().webContents.send(ON_ALGORITHIM_END, {
      path: roverPath,
      metrics: transformedMetrics,
    });
  } catch {
    if (myToken === requestToken) {
      getRendererWindow().webContents.send(ON_ALGORITHIM_END, null);
    }
  } finally {
    /* clear the slot only if we’re still the latest request */
    if (myToken === requestToken) activeRun = null;
  }
});

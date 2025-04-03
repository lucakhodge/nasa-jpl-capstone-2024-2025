import { exec } from 'child_process';
import { app, ipcMain } from "electron";
import path from 'path';
import { promisify } from 'util';
import fs from 'fs';
import { CALL_ALGORITHIM, Parameters } from './electronIPC';
import { getDemFilePath } from './FilePathStore';

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
  flagsStr += " --radius " + 100
  flagsStr += " --iterations " + 1
  flagsStr += " --json true "
  return flagsStr;
}

ipcMain.handle(CALL_ALGORITHIM, async (_event, parameters: Parameters) => {

  console.log("in call algo handle, was passed:", parameters);
  const outputPath = path.join(app.getPath("temp"), 'path-result');

  try {
    const executableCall = getExecutablePath() + getFlags(parameters, getDemFilePath(), outputPath);
    console.log("EC: ", executableCall)

    const { stderr } = await execPromise(executableCall);
    if (stderr) {
      console.log("IN STDERR");
      throw new Error(stderr);
    }
  } catch (error) {
    console.log("IN a ERROR");
    // TODO: what should happen on error?
    return undefined;
  }

  const data = fs.readFileSync(outputPath, "utf-8");
  console.log("FILE data", data)
  return data;
});

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
  flagsStr += " -x " + parameters.startCoordinate.x
  flagsStr += " -y " + parameters.startCoordinate.y
  flagsStr += " -X " + parameters.endCoordinate.x
  flagsStr += " -Y " + parameters.endCoordinate.y
  flagsStr += " -i " + inputPath
  flagsStr += " -o " + outputPath
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
      throw new Error(stderr);
    }
  } catch (error) {
    // TODO: what should happen on error?
    return undefined;
  }

  const data = fs.readFileSync(outputPath, "utf-8");
  return data;
});

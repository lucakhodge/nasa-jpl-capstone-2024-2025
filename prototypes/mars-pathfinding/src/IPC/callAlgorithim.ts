import { exec } from 'child_process';
import { app, ipcMain } from "electron";
import path from 'path';
import { promisify } from 'util';
import fs from 'fs';
import { CALL_ALGORITHIM, Parameters } from './electronIPC';

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

  // Adjust for Vite's structure
  return path.join(basePath, executableName);
};

ipcMain.handle(CALL_ALGORITHIM, async (event, parameters: Parameters) => {

  console.log("in call algo handle, was passed:", parameters);
  let outputDir = app.getPath("temp");
  let outputPath = path.join(outputDir, 'path-result');
  // let outputPath = "/Users/lucahodge/Boulder/capstone/nasa-jpl-capstone-2024-2025/prototypes/mars-pathfinding/test";

  try {

    let executableCall = getExecutablePath() + " -x 10 -y 15 -X 20 -Y 30 -o " + outputPath
    console.log("ec ", executableCall)

    const { stderr } = await execPromise(executableCall);
    if (stderr) {
      throw new Error(stderr);
    }
    // console.log(stdout);
    // return stdout;
  } catch (error) {
    // return `Error: ${error.message}`;
    // return `Error: ${error}`;
    return undefined;
  }

  const data = fs.readFileSync(outputPath, "utf-8");
  return data;
});

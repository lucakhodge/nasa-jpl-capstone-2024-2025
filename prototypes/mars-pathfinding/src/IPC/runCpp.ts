import { exec } from 'child_process';
import { app, ipcMain } from "electron";
import path from 'path';
import { promisify } from 'util';
import { CALL_CPP } from "./electronIPC";

const execPromise = promisify(exec)

const getExecutablePath = () => {
  const basePath = app.isPackaged ? path.join(process.resourcesPath, 'assets') : path.join(__dirname, '../../assets');
  // Get executable name based on platform
  let executableName = 'example';
  if (process.platform === 'win32') {
    executableName = 'example-win';
  } else if (process.platform === 'darwin') {
    executableName = 'example-darwin';
  } else if (process.platform === 'linux') {
    executableName = 'example-linux';
  }
  // Adjust for Vite's structure
  return path.join(basePath, executableName);
};

ipcMain.handle(CALL_CPP, async () => {
  console.log("in call cpp handle")
  try {
    //NOTE: the path is from where you run from
      const { stdout, stderr } = await execPromise(getExecutablePath());
      if (stderr) {
          throw new Error(stderr);
      }
      return stdout;
  } catch (error) {
      // return `Error: ${error.message}`;
      return `Error: ${error}`;
  }
});

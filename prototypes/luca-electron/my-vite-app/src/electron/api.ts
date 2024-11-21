// src/electron/api.ts

import { exec } from 'child_process';
import { ipcMain, IpcMainInvokeEvent } from "electron";
import { promisify } from 'util';

const execPromise = promisify(exec)

ipcMain.handle(
  "node-version",
  (event: IpcMainInvokeEvent, msg: string): string => {
    console.log(event);
    console.log(msg);

    return process.versions.node;
  }
);

// add to run cpp program
ipcMain.handle('run-cpp', async () => {
  try {
    //NOTE: the path is from where you run from
      const { stdout, stderr } = await execPromise('../cpp-program/example');
      if (stderr) {
          throw new Error(stderr);
      }
      return stdout;
  } catch (error) {
      // return `Error: ${error.message}`;
      return `Error: ${error}`;
  }
});
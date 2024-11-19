import { exec } from 'child_process';
import { ipcMain } from "electron";
import { promisify } from 'util';
import { CALL_CPP } from "./electronIPC";

const execPromise = promisify(exec)

ipcMain.handle(CALL_CPP, async () => {
  console.log("in call cpp handle")
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
// src/electron/main.ts

import { app, BrowserWindow } from "electron";
import { join } from "path";

// 1. this import won't work yet, but we will fix that next
import "./api";

// 2. simple check if we are running in dev / preview / production
const isDev = process.env.DEV != undefined;
const isPreview = process.env.PREVIEW != undefined;

function createWindow() {
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: join(__dirname, "preload.js"),
    },
  });

  if (isDev) {
    mainWindow.loadURL("http://localhost:5173");
                                      // ^^^^ make sure this port
                                      // matches the port used when
                                      // you run 'yarn run vite'
    // mainWindow.webContents.openDevTools();
  } else if (isPreview) {
    // mainWindow.webContents.openDevTools();
    mainWindow.loadFile("dist/index.html");
                  // 3. ^^^^^ this 'dist' folder will be our output folder
  } else {
    mainWindow.loadFile("dist/index.html");
  }
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
  createWindow();

  app.on("activate", () => {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on("window-all-closed", () => {
  if (process.platform !== "darwin") app.quit();
});
import { ipcMain } from 'electron';
import fs from 'fs/promises';
import * as fsSync from 'fs';
import os from 'os';
import path from 'path';

console.log('Setting up file reader IPC handlers');

// Existing read-file handler
ipcMain.handle('read-file', async (_, filePath: string) => {
  try {
    console.log(`Reading file: ${filePath}`);
    const data = await fs.readFile(filePath, 'utf8');
    console.log(`File read successfully, ${data.length} bytes`);
    return data;
  } catch (error) {
    console.error(`Error reading file ${filePath}:`, error);
    throw error;
  }
});

// Get temp directory handler
ipcMain.handle('get-temp-directory', () => {
  const tempDir = os.tmpdir();
  console.log(`System temp directory: ${tempDir}`);
  return tempDir;
});

// Find metrics file handler
ipcMain.handle('find-metrics-file', async () => {
  try {
    const tempDir = os.tmpdir();
    const metricsFilePath = path.join(tempDir, 'path-result');
    console.log(`Looking for metrics file at: ${metricsFilePath}`);
    
    if (fsSync.existsSync(metricsFilePath)) {
      console.log(`Metrics file found: ${metricsFilePath}`);
      return metricsFilePath;
    }
    
    throw new Error(`Metrics file not found at ${metricsFilePath}`);
  } catch (error) {
    console.error('Error finding metrics file:', error);
    throw error;
  }
});
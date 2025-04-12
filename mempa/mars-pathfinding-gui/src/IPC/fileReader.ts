import { ipcMain } from 'electron';
import fs from 'fs/promises';
import * as fsSync from 'fs';
import path from 'path';

console.log('Setting up file reader IPC handler');

// Create handler to read files with existence check
ipcMain.handle('read-file', async (_, filePath: string) => {
  try {
    console.log(`Attempting to read file: ${filePath}`);
    
    // Check if file exists before trying to read it
    if (!fsSync.existsSync(filePath)) {
      console.error(`File not found at path: ${filePath}`);
      throw new Error(`File not found: ${filePath}`);
    }
    
    const data = await fs.readFile(filePath, 'utf8');
    console.log(`File read successfully, ${data.length} bytes`);
    return data;
  } catch (error) {
    console.error(`Error reading file ${filePath}:`, error);
    throw error;
  }
});
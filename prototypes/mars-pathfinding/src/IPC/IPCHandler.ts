import { ipcMain } from 'electron';
import { exec } from 'child_process';
import * as path from 'path';
import * as fs from 'fs';

export function setupIPCHandlers() {
  ipcMain.handle('run-pathfinding', async (_, args) => {
    const { demFile, startX, startY, endX, endY, maxSlope } = args;
    
    // Path to your C++ algorithm executable
    const algorithmPath = path.join(__dirname, '../../../mempa/algorithm/build/pathfinding_cli');
    
    console.log(`Running pathfinding algorithm with:
      - DEM: ${demFile}
      - Start: (${startY},${startX}) [y,x format for C++]
      - End: (${endY},${endX}) [y,x format for C++]
      - Max Slope: ${maxSlope}°`);
    
    return new Promise((resolve, reject) => {
      // Important: Note the swapped coordinates - C++ expects y,x format
      exec(`${algorithmPath} --dem "${demFile}" --start ${startY},${startX} --end ${endY},${endX} --slope ${maxSlope}`, 
        (error, stdout, stderr) => {
          if (error) {
            console.error('Pathfinding error:', stderr);
            reject(error);
            return;
          }
          
          try {
            // Parse JSON output from the C++ algorithm
            const result = JSON.parse(stdout);
            console.log(`Path found with ${result.path.length} points`);
            resolve(result);
          } catch (err) {
            console.error('Failed to parse algorithm output:', stdout);
            reject(new Error('Failed to parse algorithm output'));
          }
        }
      );
    });
  });
}
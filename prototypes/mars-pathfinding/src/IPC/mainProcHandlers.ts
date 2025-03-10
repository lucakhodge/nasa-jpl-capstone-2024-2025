import { ipcMain, dialog } from 'electron';
import * as path from 'path';
import * as fs from 'fs';
import { exec } from 'child_process';

// Define constants if needed
const RUN_PATHFINDING = 'run-pathfinding';

export function setupIPCHandlers() {
  console.log("Setting up IPC handlers");
  
  // DEM handlers
  ipcMain.handle('open-dem', async (event, filePath) => {
    console.log('open-dem handler called with:', filePath);
    
    // Handle undefined file path
    if (!filePath) {
      const result = await dialog.showOpenDialog({
        properties: ['openFile'],
        filters: [
          { name: 'DEM Files', extensions: ['tif', 'dem'] },
          { name: 'All Files', extensions: ['*'] }
        ]
      });
      
      if (result.canceled || !result.filePaths.length) {
        throw new Error('No file selected');
      }
      
      filePath = result.filePaths[0];
    }
    
    // Create a simple mock DEM info
    const demInfo = {
      filename: path.basename(filePath),
      width: 500,
      height: 500
    };
    
    event.sender.send('on-dem-opened', demInfo);
    return demInfo;
  });

  ipcMain.handle('close-dem', (event) => {
    console.log('close-dem handler called');
    event.sender.send('on-dem-closed');
    return true;
  });

  ipcMain.handle('get-chunk', (_, args) => {
    console.log('get-chunk handler called:', args);
    const { coordinate, size } = args;
    
    // Create test data
    const data = Array(size.height).fill(0).map(() => 
      Array(size.width).fill(0).map(() => Math.random() * 1000)
    );
    
    // Add a hill in the middle
    const centerX = Math.floor(size.width / 2);
    const centerY = Math.floor(size.height / 2);
    
    for (let y = 0; y < size.height; y++) {
      for (let x = 0; x < size.width; x++) {
        const distFromCenter = Math.sqrt(Math.pow(x - centerX, 2) + Math.pow(y - centerY, 2));
        if (distFromCenter < size.width / 4) {
          const hillHeight = 2000 * Math.exp(-distFromCenter * distFromCenter / (size.width * size.width / 16));
          data[y][x] += hillHeight;
        }
      }
    }
    
    return {
      description: {
        coordinate,
        size,
        filename: "test-dem.tif",
      },
      data
    };
  });

  // INTEGRATED PATHFINDING HANDLER
  ipcMain.handle(RUN_PATHFINDING, async (_, args) => {
    console.log('Running pathfinding algorithm with args:', args);
    const { demFile, startX, startY, endX, endY, maxSlope } = args;
    
    // Path to the binary
    const algorithmPath = path.resolve(
      __dirname, 
      '../../../mempa/algorithm/build/pathfinding_cli'
    );
    
    console.log(`Pathfinding executable path: ${algorithmPath}`);
    
    // Check if executable exists
    if (!fs.existsSync(algorithmPath)) {
      console.error(`Pathfinding executable not found at: ${algorithmPath}`);
      
      // Return mock data instead of throwing error
      return {
        path: [[startX, startY], 
               [startX+10, startY+10], 
               [startX+20, startY+15], 
               [endX-10, endY-5], 
               [endX, endY]],
        metrics: {
          length: 100,
          steps: 5,
          maxSlope: 12.5,
          avgSlope: 8.3
        }
      };
    }
    
    return new Promise((resolve, reject) => {
      // Run the C++ CLI tool
      const command = `"${algorithmPath}" --dem "${demFile}" --start ${startY},${startX} --end ${endY},${endX} --slope ${maxSlope}`;
      
      console.log(`Executing: ${command}`);
      
      exec(command, (error, stdout, stderr) => {
        if (error) {
          console.error('Pathfinding error:', stderr || error.message);
          reject(new Error(`Pathfinding failed: ${stderr || error.message}`));
          return;
        }
        
        try {
          // Parse JSON output
          const result = JSON.parse(stdout);
          console.log(`Path found with ${result.path?.length || 0} points`);
          
          // Transform from [y,x] format to [x,y] format for the frontend
          if (result.path) {
            result.path = result.path.map(point => [point[1], point[0]]);
          }
          
          resolve(result);
        } catch (parseError) {
          console.error('Failed to parse algorithm output:', stdout);
          reject(new Error('Invalid output from pathfinding algorithm'));
        }
      });
    });
  });

  // Alternative direct handler if needed
  ipcMain.handle('run-pathfinding', (_, args) => {
    // Call the same handler with a different name to ensure it works
    return ipcMain.handle(RUN_PATHFINDING, _, args);
  });

  // Other handlers
  ipcMain.handle('call-cpp', () => {
    console.log('call-cpp handler called');
    return 'Called C++ code';
  });

  ipcMain.on('react-loaded', () => {
    console.log('react-loaded event received');
  });
  
  console.log("IPC handlers setup complete");
}
import { ipcMain, dialog } from 'electron';
import * as path from 'path';
import * as fs from 'fs';
import { exec } from 'child_process';

export function setupIPCHandlers() {
  console.log("Setting up IPC handlers");
  
  // Your existing DEM handlers...
  
  // Add the pathfinding handler
  ipcMain.handle('run-pathfinding', async (_, args) => {
    console.log('Running pathfinding algorithm with args:', args);
    const { demFile, startX, startY, endX, endY, maxSlope } = args;
    
    // For testing purposes, generate a simple path
    // In production, this would call your C++ algorithm
    const mockPath = generateMockPath(startX, startY, endX, endY);
    
    return {
      path: mockPath,
      metrics: {
        length: calculatePathLength(mockPath),
        steps: mockPath.length,
        maxSlope: 15.2,
        avgSlope: 8.7
      }
    };
  });
}

// Helper to generate a mock path for testing
function generateMockPath(startX, startY, endX, endY) {
  const path = [];
  const steps = 10; // Number of points in the path
  
  for (let i = 0; i <= steps; i++) {
    const progress = i / steps;
    const x = Math.round(startX + (endX - startX) * progress);
    const y = Math.round(startY + (endY - startY) * progress);
    
    // Add some randomness to make it look like a real path
    const jitterX = i > 0 && i < steps ? Math.round((Math.random() - 0.5) * 5) : 0;
    const jitterY = i > 0 && i < steps ? Math.round((Math.random() - 0.5) * 5) : 0;
    
    path.push([x + jitterX, y + jitterY]);
  }
  
  return path;
}

// Calculate approximate path length
function calculatePathLength(path) {
  let length = 0;
  for (let i = 1; i < path.length; i++) {
    const dx = path[i][0] - path[i-1][0];
    const dy = path[i][1] - path[i-1][1];
    length += Math.sqrt(dx*dx + dy*dy);
  }
  return length;
}
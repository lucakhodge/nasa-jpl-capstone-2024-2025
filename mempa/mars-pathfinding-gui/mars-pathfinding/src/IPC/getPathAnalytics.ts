import { ipcMain } from 'electron';
import * as fs from 'fs';
import * as path from 'path';
import { GET_PATH_ANALYTICS } from './electronIPC';

ipcMain.handle(GET_PATH_ANALYTICS, async (_event, startX, startY, endX, endY) => {
  try {
    // Construct the path to the analytics file
    const filename = `test_${startX}_${startY}_${endX}_${endY}.log`;
    const filePath = path.join(__dirname, '../../../algorithm/test_results', filename);
    
    console.log('Looking for analytics file:', filePath);
    
    // Check if file exists
    if (!fs.existsSync(filePath)) {
      console.log(`No analytics file found for ${filename}`);
      return null;
    }
    
    // Read the file
    const data = fs.readFileSync(filePath, 'utf-8');
    
    // Parse the analytics
    const metrics = parsePathAnalytics(data);
    console.log('Found metrics:', metrics);
    return metrics;
  } catch (error) {
    console.error('Error reading path analytics:', error);
    return null;
  }
});

// Helper function to parse the analytics file
function parsePathAnalytics(fileContent) {
  const metrics = {
    totalDistance: '0',
    horizontalDistance: '0',
    totalElevationChange: '0',
    netElevationChange: '0',
    maxSlope: '0',
    avgSlope: '0',
    energyCost: '0',
  };
  
  const lines = fileContent.split('\n');
  
  for (const line of lines) {
    if (line.includes('3D Distance:')) {
      metrics.totalDistance = line.split(':')[1]?.trim().split(' ')[0] || '0';
    } else if (line.includes('2D Distance:')) {
      metrics.horizontalDistance = line.split(':')[1]?.trim().split(' ')[0] || '0';
    } else if (line.includes('Total Elevation Change:')) {
      metrics.totalElevationChange = line.split(':')[1]?.trim().split(' ')[0] || '0';
    } else if (line.includes('Maximum Slope:')) {
      metrics.maxSlope = line.split(':')[1]?.trim().split('°')[0] || '0';
    } else if (line.includes('Average Slope:')) {
      metrics.avgSlope = line.split(':')[1]?.trim().split('°')[0] || '0';
    } else if (line.includes('Energy Cost:')) {
      metrics.energyCost = line.split(':')[1]?.trim() || '0';
    }
  }
  
  return metrics;
}
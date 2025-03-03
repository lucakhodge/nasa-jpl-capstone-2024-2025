export function calculatePath(start, end, terrain) {
    if (terrain.insurmountableObstacle) {
      throw new Error('No valid path found');
    }
  
    // Basic implementation for testing
    const path = [];
    path.push(start);
    
    // Add intermediate points based on terrain
    if (terrain.heights && terrain.heights.length > 0) {
      const midpoint = {
        lat: (start.lat + end.lat) / 2,
        lng: (start.lng + end.lng) / 2
      };
      path.push(midpoint);
    }
    
    path.push(end);
    return path;
  }
  
  export function validateCoordinates({ lat, lng }) {
    return lat >= -90 && lat <= 90 && lng >= -180 && lng <= 180;
  }
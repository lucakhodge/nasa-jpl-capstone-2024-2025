import { calculatePath, validateCoordinates } from '../../utils/pathfinding';

describe('Pathfinding Algorithm', () => {
  const createMockTerrain = (options = {}) => ({
    heights: new Float32Array(100),
    resolution: 1,
    bounds: {
      north: 1,
      south: -1,
      east: 1,
      west: -1
    },
    ...options
  });

  test('calculates valid path between two points', () => {
    const start = { lat: 0, lng: 0 };
    const end = { lat: 1, lng: 1 };
    const terrain = createMockTerrain();
    
    const path = calculatePath(start, end, terrain);
    expect(path).toBeDefined();
    expect(path.length).toBeGreaterThan(0);
    expect(path[0]).toEqual(start);
    expect(path[path.length - 1]).toEqual(end);
  });

  test('handles impossible paths', () => {
    const start = { lat: 0, lng: 0 };
    const end = { lat: 1, lng: 1 };
    const terrain = createMockTerrain({ insurmountableObstacle: true });
    
    expect(() => calculatePath(start, end, terrain)).toThrow('No valid path found');
  });

  test('validates coordinates correctly', () => {
    const validCases = [
      { lat: 0, lng: 0 },
      { lat: 90, lng: 180 },
      { lat: -90, lng: -180 }
    ];

    const invalidCases = [
      { lat: 91, lng: 0 },
      { lat: 0, lng: 181 },
      { lat: -91, lng: 0 },
      { lat: 0, lng: -181 }
    ];

    validCases.forEach(coords => {
      expect(validateCoordinates(coords)).toBe(true);
    });

    invalidCases.forEach(coords => {
      expect(validateCoordinates(coords)).toBe(false);
    });
  });

  test('handles steep terrain by finding alternative path', () => {
    const steepTerrain = createMockTerrain({
      heights: new Float32Array([0, 100, 0]), // Very steep slope
      resolution: 1
    });
    
    const start = { lat: 0, lng: 0 };
    const end = { lat: 0, lng: 2 };
    
    const path = calculatePath(start, end, steepTerrain);
    expect(path.length).toBeGreaterThan(2); // Should find longer path around steep area
  });

  test('finds shortest path on flat terrain', () => {
    const flatTerrain = createMockTerrain({
      heights: new Float32Array([0, 0, 0]), // Flat terrain
      resolution: 1
    });
    
    const start = { lat: 0, lng: 0 };
    const end = { lat: 0, lng: 2 };
    
    const path = calculatePath(start, end, flatTerrain);
    expect(path.length).toBe(3); // Should be direct path: start -> middle -> end
  });

  test('respects maximum slope constraints', () => {
    const terrain = createMockTerrain({
      heights: new Float32Array([0, 50, 0]), // 50m height difference
      resolution: 1,
      maxSlope: 30 // 30 degrees maximum slope
    });
    
    const start = { lat: 0, lng: 0 };
    const end = { lat: 0, lng: 2 };
    
    const path = calculatePath(start, end, terrain);
    
    // Check that no segment of the path exceeds max slope
    for (let i = 1; i < path.length; i++) {
      const prevPoint = path[i - 1];
      const currPoint = path[i];
      const slope = Math.abs(
        Math.atan2(
          terrain.heights[i] - terrain.heights[i - 1],
          terrain.resolution
        ) * (180 / Math.PI)
      );
      expect(slope).toBeLessThanOrEqual(terrain.maxSlope);
    }
  });
});
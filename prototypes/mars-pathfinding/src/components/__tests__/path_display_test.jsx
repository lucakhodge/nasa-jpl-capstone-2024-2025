import React from 'react';
import { render, screen } from '@testing-library/react';
import PathDisplay from '../PathDisplay';

describe('Path Display', () => {
  test('updates when new path is calculated', () => {
    const initialData = {
      path: [
        { x: 1254, y: 1265, elevation: 100 },
        { x: 1340, y: 1338, elevation: 150 }
      ],
      statistics: {
        totalDistance: 150.5,
        maxSlope: 30.2,
        elevationGain: 50.0,
        timeEstimate: "2h 30m"
      }
    };

    const { rerender } = render(<PathDisplay pathData={initialData} />);
    
    // Initial assertions
    expect(screen.getByTestId('total-distance')).toHaveTextContent('150.5 meters');
    
    const newPathData = {
      path: [
        { x: 1200, y: 1200, elevation: 90 },
        { x: 1250, y: 1250, elevation: 110 }
      ],
      statistics: {
        totalDistance: 100,
        maxSlope: 25.5,
        elevationGain: 20,
        timeEstimate: "1h 15m"
      }
    };
    
    rerender(<PathDisplay pathData={newPathData} />);
    
    // Test with actual component output format
    expect(screen.getByTestId('total-distance')).toHaveTextContent('100 meters');
    expect(screen.getByTestId('max-slope')).toHaveTextContent('25.5°');
    expect(screen.getAllByTestId('path-point')).toHaveLength(2);
  });
});
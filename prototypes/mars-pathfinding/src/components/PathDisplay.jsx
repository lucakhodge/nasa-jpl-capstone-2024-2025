import React from 'react';

const PathDisplay = ({ pathData }) => {
  if (!pathData) {
    return <div>No path available</div>;
  }

  return (
    <div>
      <h2>Path Details</h2>
      <div className="path-points">
        {pathData.path.map((point, index) => (
          <div 
            key={index}
            data-testid="path-point"
          >
            {`(${point.x}, ${point.y})`}
          </div>
        ))}
      </div>

      <div className="statistics" data-testid="statistics">
        <div data-testid="total-distance">
          {pathData.statistics.totalDistance} meters
        </div>
        <div data-testid="max-slope">
          {pathData.statistics.maxSlope}°
        </div>
        <div data-testid="elevation-gain">
          {pathData.statistics.elevationGain} meters
        </div>
        <div data-testid="time-estimate">
          {pathData.statistics.timeEstimate}
        </div>
      </div>
    </div>
  );
};

export default PathDisplay;
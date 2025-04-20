import React, { useEffect, useState } from 'react'
import Map3d from './Map3d'
import { useAppSelector } from '../store/hooks'
import { selectMetrics, selectPath } from '../store/mapSlice';
import { selectDemInfo } from '../store/demSlice';


export const LoadMapChunkFromPath = () => {
  const path = useAppSelector(selectPath);
  const metrics = useAppSelector(selectMetrics);
  const demInfo = useAppSelector(selectDemInfo);

  const [chunk, setChunk] = useState(null);

  // Increase buffer size to prevent edge cutoff
  const buffer = 100; // Use larger buffer when extendedBoundary is true

  useEffect(() => {
    if (path !== null && path !== undefined) {
      const limits = path.reduce((acc, coordinate) => {
        return {
          maxX: Math.max(acc.maxX, coordinate.x),
          minX: Math.min(acc.minX, coordinate.x),
          maxY: Math.max(acc.maxY, coordinate.y),
          minY: Math.min(acc.minY, coordinate.y),
        }
      },
        {
          maxX: -Infinity,
          minX: Infinity,
          maxY: -Infinity,
          minY: Infinity,
        });

      // Add buffer around the path limits to prevent edge cutoff
      const minXWithBuffer = Math.max(0, limits.minX - buffer);
      const minYWithBuffer = Math.max(0, limits.minY - buffer);

      // Make sure we don't exceed DEM boundaries
      const maxXWithBuffer = Math.min(demInfo.width, limits.maxX + buffer);
      const maxYWithBuffer = Math.min(demInfo.height, limits.maxY + buffer);

      const width = maxXWithBuffer - minXWithBuffer;
      const height = maxYWithBuffer - minYWithBuffer;

      console.log("coord width with buffer: ", minXWithBuffer, minYWithBuffer, width, height);
      console.log("Using buffer size:", buffer);

      // Request the expanded chunk with buffer
      const chunk = window.electronIPC.getChunk({
        coordinate: {
          x: minXWithBuffer,
          y: minYWithBuffer,
        },
        dimensions: {
          width: width,
          height: height,
        },
      });

      setChunk(chunk);
    }
  }, [path, buffer, demInfo]);

  return (
    path ?
      <Map3d
        chunk={chunk}
        path={path}
      />
      :
      <div>Loading</div>
  )
}

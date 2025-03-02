import React, { useEffect, useState } from 'react'
import Map3d from './Map3d'
import { useAppSelector } from '../store/hooks'
import { selectPath } from '../store/mapSlice';
import { selectDemInfo } from '../store/demSlice';

export const LoadMapChunkFromPath = (props: {}) => {
  const path = useAppSelector(selectPath);
  const demInfo = useAppSelector(selectDemInfo);

  const [chunk, setChunk] = useState(null);

  const buffer = 50;

  useEffect(() => {
    if (path !== null) {
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
        })

      const xCoordinate = Math.max(0, limits.minX - buffer);
      const yCoordinate = Math.max(0, limits.minY - buffer);

      const width = Math.min(demInfo.width, limits.maxX + buffer) - xCoordinate
      const height = Math.min(demInfo.height, limits.maxY + buffer) - yCoordinate

      console.log("coord width ", xCoordinate, yCoordinate, width, height);

      const chunk = window.electronIPC.getChunk({
        coordinate: {
          x: xCoordinate,
          y: yCoordinate,
        },
        size: {
          width: width,
          height: height,
        },
      });
      setChunk(chunk)
    }
  }, [path])
  return (
    <Map3d chunk={chunk} path={path}></Map3d>
  )
}

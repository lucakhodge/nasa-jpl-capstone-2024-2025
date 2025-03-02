import React, { useEffect, useState } from 'react'
import Map3d from './Map3d'
import { useAppSelector } from '../store/hooks'
import { selectPath } from '../store/mapSlice';

export const LoadMap = (props: {}) => {
  const path = useAppSelector(selectPath);

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
      const chunk = window.electronIPC.getChunk({
        coordinate: {
          x: limits.minX - buffer,
          y: limits.minY - buffer,
        },
        size: {
          width: limits.maxX - limits.minX + 2 * buffer,
          height: limits.maxY - limits.minY + 2 * buffer,
        },
      });
      setChunk(chunk)
    }
  }, [path])
  return (
    <Map3d chunk={chunk} path={path}></Map3d>
  )
}

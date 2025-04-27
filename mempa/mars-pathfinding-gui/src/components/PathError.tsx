import React from 'react'
import { MyButton } from './MyButton'

interface PathErrorPropsI {
  onBack: () => void;
}
export const PathError = (props: PathErrorPropsI) => {
  return (
    <div className="w-full flex flex-col items-center justify-center h-full p-6 text-center">
      <div className="text-yellow-300 text-4xl mb-4">⚠️</div>
      <h3 className="text-lg font-semibold text-white mb-2">No Path Found</h3>
      <p className="text-gray-300 mb-4">
        The algorithm couldn't find a valid path between your selected points.
      </p>
      <div className="bg-gray-800 p-4 rounded-lg border border-gray-700 w-full max-w-md">
        <h4 className="font-medium text-blue-300 mb-2">Try these solutions:</h4>
        <ul className="text-sm text-gray-300 text-left list-disc pl-5 space-y-1">
          <li>Increase the slope tolerance</li>
          <li>Select points that are closer together</li>
          <li>Choose points with less extreme elevation differences</li>
          <li>Verify that a path is physically possible in the selected region</li>
        </ul>
      </div>
      <MyButton
        // className="mt-6"
        onClick={props.onBack}
      >
        Adjust Parameters
      </MyButton>
    </div>
  )
}

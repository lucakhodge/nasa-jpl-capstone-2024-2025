import { selectDemInfo } from '../store/demSlice';
import { useAppDispatch, useAppSelector } from '../store/hooks';
import { setMetrics, setPath } from '../store/mapSlice';
import { selectParameters } from '../store/paramatersSlice';
import { MyButton } from './MyButton';

interface GeneratePathButtonPropsI {
  children?: React.ReactNode,
  onClick?: () => void,
}

export default function GeneratePathButton(props: GeneratePathButtonPropsI) {
  const dispatch = useAppDispatch();
  const paramaters = useAppSelector(selectParameters);
  const demInfo = useAppSelector(selectDemInfo);

  const isDisabled = demInfo === null
    || paramaters.startCoordinate.x === null
    || paramaters.startCoordinate.y === null
    || paramaters.endCoordinate.x === null
    || paramaters.endCoordinate.y === null
    || paramaters.slope === null


  const generatePath = async () => {
    // call algorithim, set path
    dispatch(setPath(null));
    let algorithimResult = await window.electronIPC.callAlgorithim(paramaters)
    console.log("algorithimResult", algorithimResult)
    if (algorithimResult) {
      let path = JSON.parse(algorithimResult).data;
      let metrics = JSON.parse(algorithimResult).metrics;
      const transformedMetrics = {
        totalDistance: metrics.totalDistance,
        elevationGain: metrics.elevationGain,
        elevationLoss: metrics.elevationLoss,
        maxSlope: metrics.maxSlope,
        averageSlope: metrics.averageSlope,
        maxElevation: 0,
        minElevation: 0,
        baseElevation: 0,
        asTheCrowFlysDistance: metrics.asTheCrowFlysDistance
      };
      dispatch(setPath(path));
      dispatch(setMetrics(transformedMetrics));
    }
  }

  return (
    <MyButton onClick={() => {
      generatePath()
      if (props.onClick) {
        props.onClick()
      }
    }} disabled={isDisabled}>Generate Path</MyButton>
  )
}


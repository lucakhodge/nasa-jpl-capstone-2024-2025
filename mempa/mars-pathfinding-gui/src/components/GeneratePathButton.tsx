import { selectDemInfo } from '../store/demSlice';
import { useAppDispatch, useAppSelector } from '../store/hooks';
import { setPath } from '../store/mapSlice';
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
      dispatch(setPath(path));
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


import { selectDemInfo } from '../store/demSlice';
import { useAppDispatch, useAppSelector } from '../store/hooks';
import { setPath } from '../store/mapSlice';
import { selectParameters } from '../store/paramatersSlice';
import { MyButton } from './MyButton';

export default function GeneratePathButton() {
  const dispatch = useAppDispatch();
  const paramaters = useAppSelector(selectParameters);
  const demInfo = useAppSelector(selectDemInfo);

  const isDisabled = demInfo === null
    || paramaters.startCoordinate.x === null
    || paramaters.startCoordinate.y === null
    || paramaters.endCoordinate.x === null
    || paramaters.endCoordinate.y === null
    || paramaters.slope === null


  const handleClick = async () => {
    // call algorithim, set path
    let algorithimResult = await window.electronIPC.callAlgorithim(paramaters)
    let path = JSON.parse(algorithimResult).data;
    dispatch(setPath(path));
  }

  return (
    <MyButton onClick={handleClick} disabled={isDisabled}>Generate Path</MyButton>
  )
}


import { MyButton } from '../components/MyButton';

interface MapPagePropsI {
  onBack: () => void;
}

export default function (props: MapPagePropsI) {
  return (
    <div className="w-screen h-screen flex flex-col gap-4">
      <div className="mt-auto flex justify-between p-4">
        <MyButton onClick={props.onBack}>Back</MyButton>
      </div>
    </div>
  )
}


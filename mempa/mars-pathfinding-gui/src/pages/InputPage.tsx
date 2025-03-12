import { MyButton } from "../components/MyButton";

interface InputPagePropsI {
  onNext: () => void;
  onBack: () => void;
}

export default function (props: InputPagePropsI) {
  return (
    <div className="w-screen h-screen flex flex-col gap-4">
      <div className="mt-auto flex justify-between p-4">
        <MyButton onClick={props.onBack}>Back</MyButton>
        <MyButton onClick={props.onNext}>Next</MyButton>
      </div>
    </div>
  )
}



interface MyButtonPropsI {
  children?: React.ReactNode,
  onClick?: () => void,
  disabled?: boolean,
}

export const MyButton = (props: MyButtonPropsI) => {
  const isDisabled = props.disabled;

  if (isDisabled) {
    return (
      <button disabled className='bg-slate-500 px-2 rounded  font-bold overflow-hidden'>
        {props.children}
      </button >
    )
  }
  else {

    return (
      <button className='bg-slate-300 px-2 rounded hover:bg-slate-400 font-bold overflow-hidden'
        onClick={props.onClick}>
        {props.children}
      </button>
    )
  }
}

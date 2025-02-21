
interface MyButtonPropsI {
  children?: React.ReactNode,
  onClick?: () => void,
}

export const MyButton = (props: MyButtonPropsI) => {
  return (
    <button className='bg-slate-300 rounded hover:bg-slate-400 font-bold overflow-hidden' onClick={props.onClick}>{props.children}</button>
  )
}

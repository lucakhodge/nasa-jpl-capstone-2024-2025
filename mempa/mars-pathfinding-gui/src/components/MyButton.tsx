
interface MyButtonPropsI {
  children?: React.ReactNode,
  onClick?: () => void,
  disabled?: boolean,
}

export const MyButton = (props: MyButtonPropsI) => {
  const isDisabled = props.disabled;

  return (<button
    onClick={props.onClick}
    disabled={isDisabled}
    className="
    px-3 py-1 rounded-md font-medium transition-colors
    bg-gray-700 text-gray-100 border border-gray-600
    hover:bg-gray-600
    focus:outline-none focus:ring-2 focus:ring-blue-400 focus:ring-offset-1

    disabled:bg-gray-600 disabled:text-gray-400
    disabled:border-gray-500 disabled:opacity-60
    disabled:cursor-not-allowed
  "
  >
    {props.children}
  </button>
  )
}

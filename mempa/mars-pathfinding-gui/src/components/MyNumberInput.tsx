import React, { forwardRef } from 'react'

interface MyNumberInputPropsI {
  children?: React.ReactNode,
  value: number | null,
  onChange: (value: number) => void,
  onEnter?: () => void,
  disabled?: boolean,
}

export const MyNumberInput = forwardRef<HTMLInputElement, MyNumberInputPropsI>((props: MyNumberInputPropsI, ref) => {
  const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    if (event.target.value === "") {
      props.onChange(null)
    }
    else {
      const val = Number(event.target.value)
      props.onChange(isNaN(val) ? null : val)
    }
  }

  const handleKeyDown = (event: React.KeyboardEvent<HTMLInputElement>) => {
    if (event.key === 'Enter' && props.onEnter) {
      props.onEnter();
    }
  };

  return (
    <div>
      <input
        ref={ref}
        type="number"
        value={props.value ?? ""}
        onChange={handleChange}
        onKeyDown={handleKeyDown}
        disabled={props.disabled}
        className={
          `
      w-24 px-2 py-1 rounded-md font-medium transition-colors
      ${props.disabled
            ? "bg-gray-600 text-gray-400 border border-gray-500 opacity-60 cursor-not-allowed"
            : "bg-gray-700 text-gray-100 border border-gray-600 hover:bg-gray-600"}
      ${props.disabled ? "" :
            "focus:outline-none focus:ring-2 focus:ring-blue-400 focus:border-blue-400"}
      appearance-none
    `
        }
      />
    </div>
  );
  if (props.disabled) {
    return (
      <div>
        <input ref={ref} onChange={handleChange} type='number' value={props.value == null ? "" : props.value} disabled className='w-20 px-2 bg-slate-500 rounded font-bold' />
      </div>
    )
  }
  else {
    return (
      <div>
        <input ref={ref} onChange={handleChange} onKeyDown={handleKeyDown} type='number' value={props.value == null ? "" : props.value}
          className="
    w-24
    px-2 py-1
    bg-gray-700             
    text-gray-100
    border border-gray-600 
    rounded-md            
    font-medium          
    placeholder-gray-500
    hover:bg-gray-600
    focus:outline-none
    focus:ring-2 focus:ring-blue-400 focus:border-blue-400
    transition-colors
    appearance-none     
  "
        />
      </div>
    )
  }
}
)

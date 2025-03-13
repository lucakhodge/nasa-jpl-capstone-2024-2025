import React from 'react'

interface MyNumberInputPropsI {
  children?: React.ReactNode,
  value: number | null,
  onChange: (value: number) => void,
  disabled?: boolean,
}

export const MyNumberInput = (props: MyNumberInputPropsI) => {
  const handleChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    if (event.target.value === "") {
      props.onChange(null)
    }
    else {
      const val = Number(event.target.value)
      props.onChange(isNaN(val) ? null : val)
    }
  }
  if (props.disabled) {
    return (
      <div>
        <input onChange={handleChange} type='number' value={props.value == null ? "" : props.value} disabled className='w-20 px-2 bg-slate-500 rounded font-bold' />
      </div>
    )
  }
  else {
    return (
      <div>
        <input onChange={handleChange} type='number' value={props.value == null ? "" : props.value} className='w-20 px-2 bg-slate-300 rounded hover:bg-slate-400 font-bold' />
      </div>
    )
  }
}

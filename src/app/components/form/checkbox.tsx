import { ReactNode, useId } from "react";

import "./checkbox.scss";

interface CheckboxProps {
	value: boolean;
	children: ReactNode;
	onInput: (v: boolean) => void;
}

export function Checkbox({ value, children, onInput }: CheckboxProps) {
	const id = useId();
	return (
		<div className="form-check">
			<input className="form-check-input" type="checkbox" id={id}
				checked={value} onChange={e => onInput(e.currentTarget.checked)} />
			<label className="form-check-label" htmlFor={id}>
				{children}
			</label>
		</div>
	)
}
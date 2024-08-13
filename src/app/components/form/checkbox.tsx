import { ReactNode, useId } from "react";

interface CheckboxProps {
	value: boolean;
	disabled?: boolean;
	className?: string;
	children?: ReactNode;
	onInput: (v: boolean) => void;
}

export function Checkbox({ value, className, disabled, children, onInput }: CheckboxProps) {
	const id = useId();
	return (
		<div className={"form-check " + className}>
			<input
				className="form-check-input"
				disabled={disabled}
				type="checkbox"
				id={id}
				checked={value}
				onChange={e => onInput(e.currentTarget.checked)}
			/>
			<label className="form-check-label" htmlFor={id}>
				{children}
			</label>
		</div>
	);
}

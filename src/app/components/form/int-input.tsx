import { useEffect, useState } from "react";
import { useTranslation } from "react-i18next";

export interface InputProps<T> {
	max?: number;
	value: T;
	onInput: (v: T) => void;
}

export interface IntInputProps extends InputProps<number> {
	min?: number;
}

export function IntInput({ max, min, value, onInput }: IntInputProps) {
	const { t } = useTranslation();
	if(min === undefined) min = 0;
	const [temp, setTemp] = useState(value.toString());

	function isValid(v = Number(temp)): boolean {
		return Number.isSafeInteger(v) && v >= min! && (max === undefined || v <= max);
	}

	function input(exp: string): void {
		setTemp(exp);
		const v = Number(exp);
		if(isValid(v)) onInput(v);
	}

	function blur() {
		if(!isValid()) setTemp(value.toString());
		else setTemp(Number(value).toString());
	}

	useEffect(() => {
		if(Number(temp) != value) setTemp(value.toString());
	}, [value]);

	return (
		<div className="position-relative">
			<div className="exp-container">
				<input
					type="number"
					aria-label="Integer"
					className={"form-control exp-input " + (isValid() ? "" : "is-invalid")}
					value={temp}
					onInput={e => input(e.currentTarget.value)}
					onBlur={blur}
				/>
			</div>
			{!isValid() &&
				<div className="text-danger small">
					{max !== undefined ? t("invalid.intMax", { min, max }) : t("invalid.int", { min })}
				</div>
			}
		</div>
	);
}

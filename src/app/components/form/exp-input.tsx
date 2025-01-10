import { useEffect, useRef, useState } from "react";
import { useTranslation } from "react-i18next";
import { parse } from "../../parser";
import { useSettings, useStore } from "../../store";

import "./exp-input.scss";

import type { InputProps } from "./int-input";

interface ExpInputProps extends Omit<InputProps<number>, "onInput"> {
	exp?: string;
	onInput: (v: number, exp: string) => void;
}

const canvas: HTMLCanvasElement = document.createElement("canvas");

/**
 * Uses canvas.measureText to compute and return the width of the given text of given font in pixels.
 * @see https://stackoverflow.com/questions/118241/calculate-text-width-with-javascript/21015393#21015393
 */
function getTextWidth(input: HTMLInputElement, text: string): number {
	const context = canvas.getContext("2d")!;
	context.font = getComputedStyle(input).font;
	const metrics = context.measureText(text);
	return metrics.width;
}

export function ExpInput({ max, value, exp, onInput }: ExpInputProps) {
	const { t } = useTranslation();
	const el = useRef<HTMLInputElement>(null);
	const settings = useSettings();
	const suffix = useRef<HTMLSpanElement>(null);
	const [temp, setTemp] = useState(exp ?? value.toString());
	const [tempValue, setTempValue] = useState(value);
	const [cache, setCache] = useState(exp ?? value.toString());
	const store = useStore();

	function format(v: number): string {
		return v.toFixed(settings.precision).replace(/0+$/, "").replace(/\.$/, "").replace(/^$/, "0");
	}

	function isValid(v = tempValue): boolean {
		return !isNaN(v) && v >= 0 && (max === undefined || v <= max);
	}

	function input(exp: string): void {
		setTemp(exp);
		const v = parse(exp);
		setTempValue(v);
		if(isValid(v)) {
			setCache(exp);
			onInput(v, exp);
		}
	}

	function blur() {
		if(!isValid()) {
			setTemp(cache);
			setTempValue(value);
		}
	}

	function shouldShowSuffix() {
		return isNaN(Number(temp)) && temp != tempValue.toString() && !isNaN(tempValue);
	}

	useEffect(() => {
		if(value == tempValue && (exp === undefined || exp == temp)) return;
		setTemp(exp ?? value.toString());
		setTempValue(value);
	}, [value, exp]);

	useEffect(() => {
		if(!isValid(value)) {
			// This happens when sheet size changes
			setTemp("0");
			setTempValue(0);
			onInput(0, "0");
		}
	});

	useEffect(() => {
		suffix.current!.style.left = getTextWidth(el.current!, temp) + "px";
	});

	return <>
		<div className="position-relative">
			<div className="exp-container">
				<input
					ref={el}
					type="text"
					aria-label="Math expression"
					className={"form-control exp-input " + (isValid() ? "" : "is-invalid")}
					value={temp}
					onInput={e => input(e.currentTarget.value)}
					onBlur={blur}
				/>
				<div className="suffix-container d-none d-sm-block">
					<span className="exp-suffix" ref={suffix}>
						{shouldShowSuffix() ? `= ${format(tempValue)}` : ""}
					</span>
				</div>
			</div>
			<div className="exp-suffix-sm d-block d-sm-none">
				<span>{shouldShowSuffix() ? `= ${format(tempValue)}` : ""}</span>
			</div>
		</div>
		{!isValid() &&
			<div className="text-danger small mt-1">
				{max !== undefined ? t("invalid.expMax", { max }) : t("invalid.exp")}
			</div>
		}
	</>;
}

import { useTranslation } from "react-i18next";
import { useDB } from "../store";
import { ExpInput } from "./exp-input";
import { InputProps } from "./int-input";

interface ICoordinates {
	x: number;
	y: number;
}

interface PointInputProps extends InputProps<ICoordinates> {
	label: string;
}

export function PointInput({ label, value, onInput }: PointInputProps) {
	const { t } = useTranslation();
	const db = useDB();
	return (
		<div className="row gx-2 mt-2">
			<label className="col-auto col-form-label capitalize">
				{t("phrase.point")}{label}: (<span className="d-none d-sm-inline"> x =</span>
			</label>
			<div className="col">
				<ExpInput max={db.width} value={value.x} onInput={v => onInput({ ...value, x: v })} />
			</div>
			<label className="col-auto col-form-label">
				,<span className="d-none d-sm-inline"> y =</span>
			</label>
			<div className="col">
				<ExpInput max={db.height} value={value.y} onInput={v => onInput({ ...value, y: v })} />
			</div>
			<label className="col-auto col-form-label">
				)
			</label>
		</div>
	);
}
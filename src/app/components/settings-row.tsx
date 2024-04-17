import { ReactNode } from "react";

interface SettingsRowProps {
	label: string;
	children: ReactNode;
}

export function SettingsRow({ label, children }: SettingsRowProps) {
	return (
		<div className="row mb-1">
			<label className="col-12 col-sm-5 col-form-label">{label}</label>
			<div className="col">
				{ children }
			</div>
		</div>
	);
}
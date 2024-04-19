import { ReactNode } from "react";

interface SettingsRowProps {
	label: string;
	children: ReactNode;
}

export function SettingsRow({ label, children }: SettingsRowProps) {
	return (
		<>
			<label className="col-form-label">{label}</label>
			<div>
				{children}
			</div>
		</>
	);
}

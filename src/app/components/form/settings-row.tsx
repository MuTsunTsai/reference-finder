import { InfoTooltip } from "../tooltip";

import type { ReactNode } from "react";

interface SettingsRowProps {
	label: string;
	help?: string;
	children: ReactNode;
}

export function SettingsRow({ label, children, help }: SettingsRowProps) {
	return (
		<>
			<label className="col-form-label">
				{label}{help && <InfoTooltip title={help} />}
			</label>
			<div>
				{children}
			</div>
		</>
	);
}

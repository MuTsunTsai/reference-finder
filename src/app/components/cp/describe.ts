import { useTranslation } from "react-i18next";

import { getSelectionPoints } from "../../cp/parse";
import { useSettings } from "../../store";

import type { CpSelection, FittedPattern } from "../../cp/parse";

/** Returns a function that describes the selection with its coordinates. */
export function useDescribeSelection(cp: FittedPattern | null) {
	const { t } = useTranslation();
	const { precision } = useSettings();

	function format(v: number): string {
		return v.toFixed(precision).replace(/\.?0+$/, "");
	}

	return (selection: CpSelection | null): string | null => {
		if(!cp || !selection) return null;
		const [p1, p2] = getSelectionPoints(cp, selection);
		if(!p2) return t("cp.point", { x: format(p1[0]), y: format(p1[1]) });
		return t("cp.line", { x1: format(p1[0]), y1: format(p1[1]), x2: format(p2[0]), y2: format(p2[1]) });
	};
}

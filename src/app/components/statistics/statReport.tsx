
import { useTranslation } from "react-i18next";
import { StatDist } from "./statDist";
import { useState } from "react";

import type { StatData } from "./chart";
import { StatSummary } from "./statSummary";

interface StatReportProps {
	data: StatData | null;
}

console.log("Stat loaded");

export default function StatReport({ data }: StatReportProps) {
	const { t } = useTranslation();
	const [tab, setTab] = useState(0);

	if(data == null) return <></>;

	return <>
		<ul className="nav nav-tabs mt-3">
			<li className="nav-item">
				<span className={"nav-link " + (tab == 0 ? "active" : "")} onClick={() => setTab(0)}>{t("statistics.summary._")}</span>
			</li>
			<li className="nav-item">
				<span className={"nav-link " + (tab == 1 ? "active" : "")} onClick={() => setTab(1)}>{t("statistics.errorDist")}</span>
			</li>
		</ul>
		<div className={"mt-3 " + (tab == 0 ? "" : "d-none")}>
			<StatSummary {...data} />
		</div>
		<div className={"mt-2 " + (tab == 1 ? "" : "d-none")}>
			<StatDist {...data} />
		</div>
	</>;
}


import { useTranslation } from "react-i18next";
import { useSettings } from "../../store";
import { StatData, getMinMax } from "./chart";

import "./statSummary.scss";

function getAverage(data: number[]): number {
	const total = data.reduce((v, x) => v + x, 0);
	return total / data.length;
}

function getStandardDeviation(data: number[], avg: number): number {
	const deviations = data.map(v => v - avg);
	const sumSquares = deviations.reduce((v, x) => v + x * x, 0);
	return Math.sqrt(sumSquares / data.length);
}

const PERCENTILES = [10, 20, 50, 80, 90, 95, 99];

export function StatSummary(props: StatData) {
	const { t } = useTranslation();
	const settings = useSettings();

	function toFixed(v: number, ex = 0): string {
		return v.toFixed(settings.precision + ex);
	}

	const [min, max] = getMinMax(props.data);
	const average = getAverage(props.data);
	const stdDev = getStandardDeviation(props.data, average);

	return (<>
		<table className="table table-striped">
			<tbody>
				<tr>
					<td>{t("statistics.summary.total")}</td>
					<td>{props.data.length}</td>
				</tr>
				<tr>
					<td>{t("statistics.summary.runTime")}</td>
					<td>{(props.time / 1000).toFixed(1)}s ({(props.time/props.data.length).toFixed(1)}ms/trial)</td>
				</tr>
				<tr>
					<td>Min / max errors</td>
					<td>{toFixed(min)} / {toFixed(max)}</td>
				</tr>
				<tr>
					<td>{t("statistics.summary.average")}</td>
					<td>{toFixed(average, 2)}</td>
				</tr>
				<tr>
					<td>{t("statistics.summary.stdDev")}</td>
					<td>{toFixed(stdDev, 2)}</td>
				</tr>
				{PERCENTILES.map((v, i) => (
					<tr key={i}>
						<td>{v}th percentile</td>
						<td>{toFixed(props.data[Math.floor(v / 100 * props.data.length)])}</td>
					</tr>
				))}
			</tbody>
		</table>
	</>);
}

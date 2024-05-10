
import { Bar } from "react-chartjs-2";
import { color } from "chart.js/helpers";
import { Theme, useSettings, useStore } from "../../store";
import { getMinMax, usePrimary, useTheme } from "./chart";
import { BUCKET_COUNT } from "./chart";

import type { TRBL } from "chart.js/dist/types/geometric";
import type { ChartData, ChartOptions } from "chart.js";
import type { StatData } from "./chart";

function getCounts(data: number[], buckets: number[]): number[] {
	const counts: number[] = [];
	let count = 0, cursor = 0;
	for(const err of data) {
		while(buckets[cursor] < err) {
			counts.push(count);
			count = 0;
			cursor++;
		}
		count++;
	}
	counts.push(count);
	return counts;
}

function getAggregates(counts: number[]): number[] {
	const result: number[] = [];
	for(const count of counts) {
		result.push(count + (result[result.length - 1] || 0));
	}
	return result.map((v, i) => v - counts[i]);
}

export function StatDist({ data }: StatData) {
	const settings = useSettings();
	const store = useStore();
	const theme = useTheme();

	const [min, max] = getMinMax(data);
	const bucketSize = (max - min) / BUCKET_COUNT;

	const buckets = Array.from({ length: BUCKET_COUNT }, (_, i) => min + (i + 1) * bucketSize);
	buckets[buckets.length - 1] = max; // prevent floating error
	const counts = getCounts(data, buckets);

	const aggregates = getAggregates(counts);

	const primary = usePrimary();

	function modify(c: string): string {
		if(theme == Theme.light) return color(c).darken(0.1).rgbString();
		return color(c).lighten(0.3).rgbString()
	}

	const chart = {
		labels: buckets.map(b => `≤ ${b.toFixed(settings.precision)}`),
		datasets: [
			{
				label: "Bucket count",
				data: counts,
				backgroundColor: "#ffc107",
				borderColor: modify("#ffc107"),
				borderWidth: 3,
			},
			{
				label: "Aggregate",
				data: aggregates,
				backgroundColor: primary,
				borderColor: modify(primary),
				borderWidth: 3,
			}
		]
	} as ChartData<"bar">;

	const options = {
		indexAxis: "y",
		maintainAspectRatio: false,
		hover: { mode: undefined }, // disable hover effect
		scales: {
			x: {
				stacked: true,
				position: "top",
			},
			y: {
				stacked: true,
				grid: { color: "transparent" },
			},
		},
		plugins: {
			legend: {
				position: "top"
			},
			datalabels: {
				anchor: "end",
				align: "right",
				display: context => context.datasetIndex || context.dataIndex == 0 ? true : "auto",
				offset: 0,
				formatter(value, context) {
					const isAgg = context.datasetIndex == 1;
					if(isAgg) {
						if(value == 0) return "";
						value += counts[context.dataIndex];
					}
					const result = `${value} (${(value / data.length * 100).toFixed(1)}%)`;
					if(isAgg && context.dataIndex == context.dataset.data.length - 1) {
						// Dynamic padding based on text width
						const width = context.chart.ctx.measureText(result).width;
						(context.chart.options.layout!.padding as TRBL).right = width + 5;
					}
					return result;
				},
			}
		}
	} as ChartOptions<"bar">;

	return (
		<div style={{ height: "25rem" }}>
			<Bar style={{ height: "100%" }} data={chart} options={options} key={store.computedTheme} />
		</div>
	);
}

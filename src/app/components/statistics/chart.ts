import ChartDataLabels from "chartjs-plugin-datalabels";
import { Chart, LinearScale, BarElement, CategoryScale, Legend } from "chart.js";
import { Theme, useStore } from "../../store";
import { color } from "chart.js/helpers";

Chart.register(CategoryScale, LinearScale, BarElement, Legend, ChartDataLabels);

export const BUCKET_COUNT = 10;

export interface StatData {
	data: number[];
	time: number;
}

export function useTheme(): Theme {
	return useStore.getState().computedTheme;
}

function bodyColor() {
	const theme = useTheme();
	return theme == Theme.dark || theme == Theme.rabbit ? "#dee2e6" : "#212529";
};

Chart.defaults.color = bodyColor;
Chart.defaults.borderColor = () => bodyColor() + "66";

export function getMinMax(data: number[]): [number, number] {
	let min = Number.POSITIVE_INFINITY;
	let max = Number.NEGATIVE_INFINITY;
	for(const x of data) {
		if(x < min) min = x;
		if(x > max) max = x;
	}
	return [min, max];
}

export function usePrimary(): string {
	const theme = useTheme();
	if(theme == Theme.rabbit) return "#198754";
	if(theme == Theme.light) return color("#0d6efd").lighten(0.5).rgbString();
	return "#0d6efd";
}


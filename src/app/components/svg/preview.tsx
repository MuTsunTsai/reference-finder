import { useDB } from "../../store";
import { Svg } from "./svg";
import { DOT_LG } from "./renderer";

import type { Renderer } from "./renderer";

interface PreviewProps {
	points?: IPoint[];
}

export function Preview({ points }: PreviewProps) {
	const { width, height } = useDB();

	function render(renderer: Renderer) {
		renderer.init();
		if(points) {
			for(const pt of points) {
				renderer.dot(pt, DOT_LG, "target-point");
			}
			if(points.length == 2) {
				const segment = renderer.clip(points[0], points[1]);
				renderer.line(segment[0], segment[1], "target-line");
			}
		}
	}

	return Svg({ render, width, height, zoom: 1, padding: 0.05, lightbox: true });
}

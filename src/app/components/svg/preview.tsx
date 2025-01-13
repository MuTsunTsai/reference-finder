import { useDB } from "../../store";
import { Svg } from "./svg";

import type { Renderer } from "./renderer";

interface PreviewProps {
	points?: IPoint[];
}

export function Preview({ points }: PreviewProps) {
	const { width, height } = useDB();

	function render(svg: Renderer) {
		// We only need to define markers once in the entire application,
		// and the container SVG needs to be visible all the time.
		// The preview SVG is then the most suitable for this.
		svg.setMarker();

		svg.init();
		if(points) {
			for(const pt of points) {
				svg.dot(pt, 0.03, "target-point");
			}
			if(points.length == 2) {
				const segment = svg.clip(points[0], points[1]);
				svg.line(segment[0], segment[1], "target-line");
			}
		}
	}

	return Svg({ render, width, height, zoom: 1, padding: 0.05, lightbox: true });
}

import ear from "rabbit-ear";
import { useDB } from "../../store";
import { Svg } from "./svg";
import { LineElement } from "../../store";

import type { RabbitEarSVG } from "rabbit-ear";

interface PreviewProps {
	cp: Fold;
	points?: IPoint[];
	existingCreases?: LineElement[];
}

export function Preview({ cp, points, existingCreases }: PreviewProps) {
	const { width, height } = useDB();
	const boundary = ear.rect(0, 0, width, height);

	function render(svg: RabbitEarSVG) {
		// We only need to define markers once in the entire application,
		// and the container SVG needs to be visible all the time.
		// The preview SVG is then the most suitable for this.
		svg.innerHTML = `<defs><marker id="arrow" viewBox="0 0 10 10" refX="5" refY="3" markerWidth="10" markerHeight="10" orient="auto-start-reverse"><path d="M 0 0 L 6 3 L 0 6 L 2 3 z" /></marker></defs>`;

		const root = svg.origami(cp);
		root.setAttribute("transform", `translate(0 ${height}) scale(1 -1)`); // lower-left origin
		if(points) {
			for(const pt of points) {
				const circle = root.vertices.circle(pt, 0.03);
				circle.classList.add("target-point");
			}
			if(points.length == 2) {
				const segment = boundary.clip(ear.line.fromPoints(points[0], points[1]));
				try {
					const line = root.edges.line(segment[0], segment[1]);
					line.classList.add("target-line");
				} catch {
					// Ignore error if the two points are identical
				}
			}
		}

		// Add existing creases from previous solutions to the preview
		if(existingCreases) {
			for(const el of existingCreases) {
				const line = root.edges.line(el.from, el.to);
				line.classList.add("line-crease");
			}
		}
	}

	return Svg({ render, width, height, padding: 0.05 });
}

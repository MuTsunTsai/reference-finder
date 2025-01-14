import { Svg } from "./svg";
import { ElementType, LabelStyle, LineStyle, PointStyle } from "../../store";
import { DOT_LG, DOT_SM } from "./renderer";

import "./diagram.scss";

import type { Renderer } from "./renderer";
import type { LabelElement, DiagramElement, PolyElement } from "../../store";

interface DiagramProps {
	data: DiagramElement[];
	padding: number;
	zoom?: number;
	last?: boolean;
	lightbox?: boolean;
}

const TEXT_OFFSET = 0.12;

/**
 * Spread out the labels from each other.
 */
function getTextOffset(text: LabelElement, data: DiagramElement[], zoom: number): IPoint {
	const offset = TEXT_OFFSET / zoom / zoom;
	const otherLabels = data.filter(e => e && e.type == ElementType.label && e != text) as LabelElement[];
	const pts = otherLabels.map(l => {
		// Other labels that are closer have greater weights
		const dx = text.pt[0] - l.pt[0];
		const dy = text.pt[1] - l.pt[1];
		const d = Math.sqrt(dx * dx + dy * dy);
		return [dx / d / d, dy / d / d];
	});
	if(pts.length == 0) return [0, 0];
	const avgX = pts.map(p => p[0]).reduce((x, v) => x + v, 0) / pts.length;
	const avgY = pts.map(p => p[1]).reduce((x, v) => x + v, 0) / pts.length;
	const norm = Math.sqrt(avgX * avgX + avgY * avgY);
	if(norm == 0) return [0, 0];
	return [offset * avgX / norm, offset * avgY / norm];
}

export function Diagram({ data, last, padding, zoom = 1, lightbox }: DiagramProps) {
	const { width, height } = data[0] as PolyElement;

	function render(renderer: Renderer) {
		renderer.init();

		for(const el of data) {
			if(!el) continue;
			if(el.type == ElementType.point) {
				const size = el.style == PointStyle.normal ? DOT_SM : DOT_LG;
				renderer.dot(el.pt, size, "point-" + PointStyle[el.style]);
			}
			if(el.type == ElementType.line) {
				const isTarget = el.style == LineStyle.valley && last;
				const className = isTarget ? "target-line" : "line-" + LineStyle[el.style];
				renderer.line(el.from, el.to, className);
			}
			if(el.type == ElementType.arc) {
				const { radius, center, from, to, ccw } = el;
				renderer.arc(center, radius, from, to, ccw, "arc-" + LineStyle[el.style]);
			}
			if(el.type == ElementType.label) {
				const offset = getTextOffset(el, data, zoom);
				renderer.text(el.text, el.pt, offset, "label-border");
				renderer.text(el.text, el.pt, offset, "label-" + LabelStyle[el.style]);
			}
		}
	}

	return Svg({ render, width, height, zoom, padding, lightbox });
}

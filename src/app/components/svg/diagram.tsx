import ear from "rabbit-ear";
import { Svg } from "./svg";

import "./diagram.scss";

import type { RabbitEarOrigami, RabbitEarSVG } from "rabbit-ear";
import { ElementType, LabelElement, LabelStyle, LineStyle, PointStyle, type DiagramElement, type PolyElement } from "../../store";

interface DiagramProps {
	data: DiagramElement[];
	last?: boolean;
}

function createText(root: RabbitEarOrigami, el: LabelElement, offset: IPoint): RabbitEarSVG {
	const text = root.edges.text(el.text, el.pt);
	text.setAttribute("transform", `translate(${offset[0]} ${2 * el.pt[1] - 0.05 + offset[1]}) scale(1 -1)`);
	return text;
}

const OFFSET_SIZE = 0.09;

/**
 * Spread out the labels from each other.
 */
function getTextOffset(text: LabelElement, data: DiagramElement[]): IPoint {
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
	return [OFFSET_SIZE * avgX / norm, OFFSET_SIZE * avgY / norm];
}

export function Diagram({ data, last }: DiagramProps) {
	const { width, height } = data[0] as PolyElement;

	function render(svg: RabbitEarSVG) {
		const root = svg.origami(ear.cp.rectangle(width, height));
		root.setAttribute("transform", `translate(0 ${height}) scale(1 -1)`); // lower-left origin
		for(const el of data) {
			if(!el) continue;
			if(el.type == ElementType.point) {
				const pt = root.edges.circle(el.pt, el.style == PointStyle.normal ? 0.02 : 0.03);
				pt.classList.add("point-" + PointStyle[el.style]);
			}
			if(el.type == ElementType.line) {
				const line = root.edges.line(el.from, el.to);
				line.classList.add(el.style == LineStyle.valley && last ? "target-line" : "line-" + LineStyle[el.style]);
			}
			if(el.type == ElementType.arc) {
				const { radius, center } = el;
				let { from, to } = el;
				if(!el.ccw) [from, to] = [to, from];

				// Apply a slight offset to the arrow to make it look better
				const max = Math.abs(to - from) / 8;
				from += Math.min(0.075 / radius, max);
				to -= Math.min(0.075 / radius, max);

				const arc = root.edges.arc(center[0], center[1], radius, from, to);
				arc.classList.add("arc-" + LineStyle[el.style]);
				if(el.ccw) arc.classList.add("reverse");
				if(radius < 0.5) {
					arc.style.strokeWidth = Math.max(0.005, 0.015 * radius / 0.5).toString();
				}
			}
			if(el.type == ElementType.label) {
				const offset = getTextOffset(el, data);
				const border = createText(root, el, offset);
				border.classList.add("label-border");

				const text = createText(root, el, offset);
				text.classList.add("label-" + LabelStyle[el.style]);
			}
		}
	}

	return Svg({ render, width, height });
}

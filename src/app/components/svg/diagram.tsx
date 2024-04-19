import ear from "rabbit-ear";
import { Svg } from "./svg";

import "./diagram.scss";

import type { RabbitEarOrigami, RabbitEarSVG } from "rabbit-ear";
import { ElementType, LabelElement, LabelStyle, LineStyle, PointStyle, type DiagramElement, type PolyElement } from "../../store";

interface DiagramProps {
	data: DiagramElement[];
	last?: boolean;
}

function parsePt(p: string): IPoint {
	return p.match(/-?\d+\.\d+/g)!.map(v => Number(v)) as IPoint;
}

// function offset(pt: IPoint, r: number, angle: number): IPoint {
// 	return [pt[0] + r * Math.cos(angle), pt[1] + r * Math.sin(angle)];
// }

function createText(root: RabbitEarOrigami, el: LabelElement): RabbitEarSVG {
	const pt = parsePt(el.pt);
	const text = root.edges.text(el.text, pt);
	text.setAttribute("transform", `translate(0 ${2 * pt[1]}) scale(1 -1)`);
	return text;
}

export function Diagram({ data, last }: DiagramProps) {
	const { width, height } = data[0] as PolyElement;

	function render(svg: RabbitEarSVG) {
		const root = svg.origami(ear.cp.rectangle(width, height));
		root.setAttribute("transform", `translate(0 ${height}) scale(1 -1)`); // lower-left origin
		for(const el of data) {
			if(!el) continue;
			if(el.type == ElementType.point) {
				const pt = root.edges.circle(parsePt(el.pt), el.style == PointStyle.normal ? 0.02 : 0.03);
				pt.classList.add("point-" + PointStyle[el.style]);
			}
			if(el.type == ElementType.line) {
				const line = root.edges.line(parsePt(el.from), parsePt(el.to));
				line.classList.add(el.style == LineStyle.valley && last ? "target-line" : "line-" + LineStyle[el.style]);
			}
			if(el.type == ElementType.arc) {
				const center = parsePt(el.center)
				const { radius } = el;
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
				const border = createText(root, el);
				border.classList.add("label-border");

				const text = createText(root, el);
				text.classList.add("label-" + LabelStyle[el.style]);
			}
		}
	}

	return Svg({ render, width, height });
}

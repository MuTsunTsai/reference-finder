import ear from "rabbit-ear";
import { Svg } from "./svg";

import "./diagram.scss";

import type { RabbitEarOrigami, RabbitEarSVG } from "rabbit-ear";
import { ElementType, LabelElement, LabelStyle, LineStyle, PointStyle, type DiagramElement, type PolyElement } from "../store";

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

function createText(root: RabbitEarOrigami, height: number, el: LabelElement): RabbitEarSVG {
	const pt = parsePt(el.pt);
	const text = root.edges.text(el.text, pt);
	text.setAttribute("transform", `translate(0 ${2 * pt[1]}) scale(1 -1)`);
	return text;
}

export function Diagram({ data, last }: DiagramProps) {
	const { width, height } = data[0] as PolyElement;

	function render(svg: RabbitEarSVG) {
		svg.innerHTML = `<defs><marker id="arrow" viewBox="0 0 10 10" refX="6" refY="3" markerWidth="10" markerHeight="10" orient="auto-start-reverse"><path d="M 0 0 L 6 3 L 0 6 L 1 3 z" /></marker></defs>`;
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
				let { from, to } = el;
				if(!el.ccw) [from, to] = [to, from];
				const arc = root.edges.arc(center[0], center[1], el.radius, from, to);
				arc.classList.add("arc-" + LineStyle[el.style]);
				if(el.ccw) arc.classList.add("reverse");
				if(el.radius < 0.5) {
					arc.style.strokeWidth = Math.max(0.005, 0.015 * el.radius / 0.5).toString();
				}
			}
			if(el.type == ElementType.label) {
				const border = createText(root, height, el);
				border.classList.add("label-border");

				const text = createText(root, height, el);
				text.classList.add("label-" + LabelStyle[el.style]);
			}
		}
	}

	return Svg({ render, width, height });
}

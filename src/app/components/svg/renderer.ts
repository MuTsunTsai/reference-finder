import ear from "rabbit-ear";

import { ElementType, useStore } from "../../store";

const MAX_ANGLE_OFFSET = Math.PI / 24; // 7.5 degree

export type RenderFunc = (svg: Renderer) => void;

/**
 * A wrapper class around {@link ear.RabbitEarSVG}.
 */
export class Renderer {

	public zoom = 1;

	private svg: ear.RabbitEarSVG;
	private width: number;
	private height: number;
	private root!: ear.RabbitEarOrigami;

	constructor(width: number, height: number, zoom: number, padding: number) {
		const svg = ear.svg();
		svg.size(width * zoom, height * zoom)
			.padding(padding)
			.strokeWidth(0.01);
		this.svg = svg;
		this.zoom = zoom;
		this.width = width;
		this.height = height;
	}

	public init() {
		this.root = this.svg.origami(ear.cp.rectangle(this.width * this.zoom, this.height * this.zoom));
		this.root.setAttribute("transform", `translate(0 ${this.height * this.zoom}) scale(1 -1)`); // lower-left origin

		// Draw existing references
		const { existingRefs } = useStore.getState();
		for(const ref of existingRefs) {
			if(ref.type == ElementType.line) this.line(ref.from, ref.to, "line-crease");
			if(ref.type == ElementType.point) this.dot(ref.pt, 0.02, "point-normal");
		}
	}

	public addTo(el: HTMLElement) {
		el.appendChild(this.svg);
	}

	public clear() {
		clear(this.svg);
	}

	public clip(from: IPoint, to: IPoint): ear.Segment {
		return ear.rect(0, 0, this.width, this.height).clip(ear.line.fromPoints(from, to));
	}

	public line(from: IPoint, to: IPoint, className: string): void {
		try {
			const line = this.root.edges.line(scale(from, this.zoom), scale(to, this.zoom));
			line.classList.add(className);
		} catch {
			// Ignore error if the two points are identical
		}
	}

	public dot(pt: IPoint, size: number, className: string): void {
		const circle = this.root.vertices.circle(scale(pt, this.zoom), size);
		circle.classList.add(className);
	}

	public arc(center: IPoint, radius: number, from: number, to: number, ccw: number, className: string): void {
		if(!ccw) [from, to] = [to, from];

		// Apply a slight offset to the arrow to make it look better
		const factor = Math.pow(this.zoom, 1.2);
		from += Math.min(0.075 / factor / radius, MAX_ANGLE_OFFSET);
		to -= Math.min(0.075 / factor / radius, MAX_ANGLE_OFFSET);

		const arc = this.root.edges.arc(
			center[0] * this.zoom, center[1] * this.zoom,
			radius * this.zoom,
			from, to
		);
		arc.classList.add(className);
		if(ccw) arc.classList.add("reverse");
		if(radius < 0.5) {
			arc.style.strokeWidth = Math.max(0.005, 0.015 * radius / 0.5).toString();
		}
	}

	public text(text: string, pt: IPoint, offset: IPoint, className: string): void {
		const label = this.root.edges.text(text, scale(pt, this.zoom));
		label.setAttribute("transform", `translate(${offset[0]} ${(2 * pt[1] * this.zoom - 0.05 + offset[1])}) scale(1 -1)`);
		label.classList.add(className);
	}
}

function scale(pt: IPoint, zoom: number): IPoint {
	return [pt[0] * zoom, pt[1] * zoom];
}

export function clear(el: Element) {
	while(el.lastChild) {
		el.removeChild(el.lastChild);
	}
}

import ear from "rabbit-ear";
import { useEffect, useRef, useState } from "react";
import { ElementType, useStore } from "../../store";

import "./svg.scss";

import type { RabbitEarOrigami, RabbitEarSVG } from "rabbit-ear";

function createRabbitEar(width: number, height: number, padding: number) {
	const svg = ear.svg();
	svg.size(width, height)
		.padding(padding)
		.strokeWidth(0.01);
	return svg;
}

interface SvgProps {
	render: (svg: RabbitEarSVG) => void;
	width: number;
	height: number;
	padding: number;
}

function clear(el: Element) {
	while(el.lastChild) {
		el.removeChild(el.lastChild);
	}
}

export function Svg({ render, width, height, padding }: SvgProps) {
	const ref = useRef<HTMLDivElement>(null);
	const [svg, setSvg] = useState<RabbitEarSVG | null>(null);

	useEffect(() => {
		if(!ref.current) return;
		clear(ref.current);
		const result = createRabbitEar(width, height, padding);
		ref.current.appendChild(result);
		setSvg(result);
	}, [ref.current, width, height]);

	useEffect(() => {
		if(!svg) return;
		clear(svg);
		render(svg);
	});

	return <div ref={ref}></div>;
}

type Transform = (pt: IPoint) => IPoint;

/**
 * Add existing creases from previous solutions to the preview.
 */
export function drawExistingRefs(root: RabbitEarOrigami, transform: Transform = p => p) {
	const { existingRefs } = useStore.getState();
	for(const ref of existingRefs) {
		if(ref.type == ElementType.line) {
			const line = root.edges.line(transform(ref.from), transform(ref.to));
			line.classList.add("line-crease");
		}
		if(ref.type == ElementType.point) {
			const circle = root.vertices.circle(transform(ref.pt), 0.02);
			circle.classList.add("point-normal");
		}
	}
}

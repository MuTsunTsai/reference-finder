import ear from "rabbit-ear";
import { useEffect, useRef, useState } from "react";

import "./svg.scss";

import type { RabbitEarSVG } from "rabbit-ear";

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

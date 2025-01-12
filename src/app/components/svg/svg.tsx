import { useEffect, useRef, useState } from "react";

import { clear, Renderer } from "./renderer";

import "./svg.scss";

interface SvgProps {
	render: (svg: Renderer) => void;
	width: number;
	height: number;
	padding: number;
}

export function Svg({ render, width, height, padding }: SvgProps) {
	const ref = useRef<HTMLDivElement>(null);
	const [svg, setSvg] = useState<Renderer | null>(null);

	useEffect(() => {
		if(!ref.current) return;
		clear(ref.current);
		const result = new Renderer(width, height, padding);
		result.addTo(ref.current);
		setSvg(result);
	}, [ref.current, width, height]);

	useEffect(() => {
		if(!svg) return;
		svg.clear();
		render(svg);
	});

	return <div ref={ref}></div>;
}

import { useEffect, useRef, useState } from "react";

import { clear, Renderer } from "./renderer";

import type { RenderFunc } from "./renderer";

import "./svg.scss";

interface SvgProps {
	render: RenderFunc;
	width: number;
	height: number;
	zoom: number;
	padding: number;
	lightbox?: boolean;
}

export function Svg({ render, width, height, zoom, padding, lightbox }: SvgProps) {
	const ref = useRef<HTMLDivElement>(null);
	const [renderer, setRenderer] = useState<Renderer | null>(null);

	const handleClick = () => {
		if(lightbox) svgHandler.showLightbox(render);
	};

	useEffect(() => {
		if(!ref.current) return;
		clear(ref.current);
		const result = new Renderer(width, height, zoom, padding);
		result.addTo(ref.current);
		setRenderer(result);
	}, [ref.current, width, height]);

	useEffect(() => {
		if(!renderer) return;
		renderer.clear();
		render(renderer);
	});

	return <div style={{ cursor: lightbox ? "zoom-in" : "unset" }} ref={ref} onClick={handleClick}></div>;
}

export const svgHandler = {
	showLightbox: async (func: RenderFunc) => { /* Nothing by default */ },
};

import { useEffect, useRef } from "react";

import "./tooltip.scss";

interface TooltipProps {
	title: string;
}

export function InfoTooltip({ title: help }: TooltipProps) {
	const tooltip = useRef<HTMLElement>(null);

	useEffect(() => {
		const el = tooltip.current;
		if(el) import("bootstrap").then(bs => bs.Tooltip.getOrCreateInstance(el));
	});

	return (
		<i className="fa-solid fa-circle-info text-info ms-2" data-bs-toggle="tooltip" data-bs-title={help} ref={tooltip}></i>
	);
}

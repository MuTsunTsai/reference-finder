import { useEffect, useRef } from "react";
import { Tooltip } from "bootstrap";

import "./tooltip.scss";

interface TooltipProps {
	title: string;
}

export function InfoTooltip({ title: help }: TooltipProps) {
	const tooltip = useRef<HTMLElement>(null);

	useEffect(() => {
		if(tooltip.current) Tooltip.getOrCreateInstance(tooltip.current);
	});

	return (
		<i className="fa-solid fa-circle-info text-info ms-2" data-bs-toggle="tooltip" data-bs-title={help} ref={tooltip}></i>
	);
}

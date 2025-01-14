import { useEffect, useRef } from "react";

import "./tooltip.scss";

interface TooltipProps {
	title: string;
	className?: string;
}

export function InfoTooltip({ title, className }: TooltipProps) {
	const tooltip = useRef<HTMLElement>(null);

	useEffect(() => {
		const el = tooltip.current;
		if(el) import("bootstrap/js/dist/tooltip").then(tp => tp.default.getOrCreateInstance(el));
	});

	return (
		<i
			className={className || "fa-solid fa-circle-info text-info ms-2"}
			data-bs-toggle="tooltip"
			data-bs-title={title}
			ref={tooltip}
		></i>
	);
}

import { useEffect, useRef } from "react";

import "./tooltip.scss";

interface TooltipProps {
	title: string;
	className?: string;
}

const loadTooltip = () => import("bootstrap/js/dist/tooltip").then(tp => tp.default);

export function InfoTooltip({ title, className }: TooltipProps) {
	const tooltip = useRef<HTMLElement>(null);

	// The title is only read when the instance is created,
	// so it has to be updated manually (e.g. when switching languages)
	useEffect(() => {
		const el = tooltip.current;
		if(!el) return;
		loadTooltip().then(Tooltip => {
			Tooltip.getOrCreateInstance(el).setContent({ ".tooltip-inner": title });
		});
	}, [title]);

	useEffect(() => {
		const el = tooltip.current;
		return () => {
			if(!el) return;
			loadTooltip().then(Tooltip => Tooltip.getInstance(el)?.dispose());
		};
	}, []);

	return (
		<i
			className={className || "fa-solid fa-circle-info text-info ms-2"}
			data-bs-toggle="tooltip"
			data-bs-title={title}
			ref={tooltip}
		></i>
	);
}

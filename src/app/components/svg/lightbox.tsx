import { useEffect, useRef, useState } from "react";
import { createPortal } from "react-dom";

import { useDB } from "../../store";
import { svgHandler, Svg } from "./svg";

import "./lightbox.scss";

import type { Renderer, RenderFunc } from "./renderer";

export function Lightbox() {
	const { height, width } = useDB();
	const [render, setRender] = useState<RenderFunc>(() => (svg: Renderer) => { });
	const ref = useRef(null);

	const show = async (func: RenderFunc) => {
		gtag("event", "ref_show_lightbox");
		setRender(() => func);
		const Modal = (await import("bootstrap/js/dist/modal")).default;
		Modal.getOrCreateInstance(ref.current!).show();
	};

	useEffect(() => {
		svgHandler.showLightbox = show;
	});

	return createPortal(
		<div className="modal fade lightbox" ref={ref}>
			<div className="modal-dialog modal-dialog-centered">
				<div className="modal-content" data-bs-dismiss="modal">
					<div className="w-100 h-100">
						{Svg({ render, height, width, zoom: 3, padding: 0.18 })}
					</div>
				</div>
			</div>
		</div>,
		document.body
	);
}

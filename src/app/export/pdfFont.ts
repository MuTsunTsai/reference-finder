export const FONT_NAME = "MyFont";

/** Compensate svg2pdf.js rendering bugs for diagram label text. */
export function patchSvgForPdf(svgEl: SVGSVGElement): void {
	for(const svg of Array.from(svgEl.querySelectorAll("svg"))) {
		for(const text of Array.from(svg.querySelectorAll("text"))) {
			const ancestor = text.closest("g[transform*='scale']");
			if(!ancestor) continue;
			const tf = ancestor.getAttribute("transform")!;
			const m = /translate\(([^\s,)]+)[\s,]+([^\s,)]+)\)/.exec(tf);
			if(!m) continue;
			// Text X gets shifted left by the ancestor translate Y value
			const offsetY = parseFloat(m[2]);
			const x = parseFloat(text.getAttribute("x") || "0");
			text.setAttribute("x", String(x + offsetY));
			// svg2pdf.js doesn't support paint-order, so split into two layers:
			// a stroke-only copy behind and a fill-only original on top
			const sw = parseFloat(text.style.strokeWidth);
			if(sw && sw < 1) {
				const border = text.cloneNode(true) as SVGTextElement;
				border.style.strokeWidth = "1px";
				border.style.fill = "none";
				text.parentElement!.insertBefore(border, text);
				text.style.stroke = "none";
				text.style.strokeWidth = "0";
			}
		}
	}
}

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export async function registerFont(pdf: any, lang: string): Promise<void> {
	const fontFile = `${lang}.ttf`;
	const url = `/reference-finder/fonts/${fontFile}`;
	try {
		const res = await fetch(url);
		if(!res.ok) return;
		const buf = await res.arrayBuffer();
		const bytes = Array.from(new Uint8Array(buf));
		const binary = bytes.map(b => String.fromCharCode(b)).join("");
		pdf.addFileToVFS(fontFile, btoa(binary));
		pdf.addFont(fontFile, FONT_NAME, "normal");
	} catch {
		// Font not available for this locale; fall back to default
	}
}

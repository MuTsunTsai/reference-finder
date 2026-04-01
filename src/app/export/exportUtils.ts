import { inlineStyles } from "./svgUtils";

// Layout constants (px)
const DIAGRAM_PX = 240;
const CELL_W = DIAGRAM_PX;
const GRID_COLS = 3;
const GAP_PX = 16;
const FONT = "13px sans-serif";
const LINE_H = 18;
const TEXT_PAD = 6;

// Layout constants (SVG units)
const CELL_SVG = 100;
const GAP_SVG = 6;

interface StepUnit {
	svg: SVGSVGElement;
	text: string;
}

/** Returns true if a character is CJK (no word boundaries). */
function isCJK(ch: string): boolean {
	const cp = ch.codePointAt(0) ?? 0;
	return (cp >= 0x3040 && cp <= 0x30FF) || // Hiragana / Katakana
		(cp >= 0x4E00 && cp <= 0x9FFF) || // CJK Unified Ideographs
		(cp >= 0x3400 && cp <= 0x4DBF) || // CJK Extension A
		(cp >= 0xF900 && cp <= 0xFAFF); // CJK Compatibility Ideographs
}

/**
 * Builds the display text for a step, prepending the step number.
 * <ol start="N"><li> renders "N." automatically in the browser, but innerText
 * does not include list markers, so we reconstruct it here.
 */
function getStepText(div: HTMLDivElement): string {
	const ol = div.querySelector("ol");
	if(!ol) return "";
	const start = parseInt(ol.getAttribute("start") ?? "1", 10);
	return Array.from(ol.querySelectorAll("li")).map((li, i) =>
		`${start + i}. ${li.innerText.trim()}`
	).join("\n");
}

/**
 * Collects (svg, text) pairs from the card-body.
 * Each step is a col-12 col-md-4 div containing an svg and an ol.
 */
export function collectSteps(cardBody: HTMLDivElement): StepUnit[] {
	const stepDivs = Array.from(cardBody.querySelectorAll<HTMLDivElement>(".col-12.col-md-4"));
	if(stepDivs.length > 0) {
		return stepDivs
			.map(
				div => ({
					svg: div.querySelector<SVGSVGElement>("svg")!,
					text: getStepText(div),
				})
			)
			.filter(u => u.svg);
	}
	// Fallback: solution with no steps (single diagram, no text)
	return Array.from(cardBody.querySelectorAll<SVGSVGElement>("svg")).map(svg => ({ svg, text: "" }));
}

// ---------------------------------------------------------------------------
// Canvas helpers
// ---------------------------------------------------------------------------

function svgToImage(svg: SVGSVGElement, theme?: "light"): Promise<HTMLImageElement> {
	const inlined = inlineStyles(svg, theme);
	const serialized = new XMLSerializer().serializeToString(inlined);
	const blob = new Blob([serialized], { type: "image/svg+xml" });
	const url = URL.createObjectURL(blob);
	return new Promise((resolve, reject) => {
		const img = new Image();
		img.onload = () => {
			URL.revokeObjectURL(url);
			resolve(img);
		};
		img.onerror = () => {
			URL.revokeObjectURL(url);
			reject(new Error("SVG load failed"));
		};
		img.src = url;
	});
}

interface WrappedLine {
	text: string;
	x: number;
}

/**
 * Wraps text to fit within maxWidth.
 * CJK characters are treated as individual break units;
 * Latin words are kept together and break at spaces.
 * The very first line (the one with "N. " prefix) starts at x=0;
 * all subsequent lines (including first lines of continuation paragraphs) start at x=indent.
 */
/** Splits a paragraph into tokens: each CJK char is its own token; Latin words carry a leading space. */
function tokenize(paragraph: string): string[] {
	const tokens: string[] = [];
	let latin = "";
	let afterCJK = false;
	for(const ch of paragraph) {
		if(isCJK(ch)) {
			if(latin) {
				tokens.push(latin);
				latin = "";
			}
			tokens.push(ch);
			afterCJK = true;
		} else if(ch === " ") {
			if(latin) {
				tokens.push(latin);
				latin = "";
			}
			if(!afterCJK) latin = " "; // attach space to next Latin word
			afterCJK = false;
		} else {
			latin += ch;
			afterCJK = false;
		}
	}
	if(latin.trim()) tokens.push(latin);
	return tokens;
}

function wrapText(ctx: CanvasRenderingContext2D, text: string, maxWidth: number, indent: number): WrappedLine[] {
	const lines: WrappedLine[] = [];
	const contWidth = maxWidth - indent;
	let isVeryFirst = true;

	for(const paragraph of text.split("\n")) {
		const tokens = tokenize(paragraph);
		let current = "";
		for(const token of tokens) {
			// Between CJK and Latin, insert a space separator
			const sep = current && isCJK(current.slice(-1)) && !isCJK(token[0]) && token[0] !== " " ? " " : "";
			const candidate = current + sep + token;
			const limit = isVeryFirst ? maxWidth : contWidth;
			if(ctx.measureText(candidate).width > limit && current) {
				lines.push({
					text: current.trimEnd(),
					x: isVeryFirst ? 0 : indent,
				});
				isVeryFirst = false;
				current = token.trimStart();
			} else {
				current = candidate;
			}
		}
		if(current.trim()) {
			lines.push({
				text: current.trimEnd(),
				x: isVeryFirst ? 0 : indent,
			});
			isVeryFirst = false;
		}
	}
	return lines;
}

async function stepToCanvas(unit: StepUnit, transparent: boolean, theme?: "light"): Promise<HTMLCanvasElement> {
	const img = await svgToImage(unit.svg, theme);

	// Pre-measure text to determine canvas height
	const measureCanvas = document.createElement("canvas");
	const measureCtx = measureCanvas.getContext("2d")!;
	measureCtx.font = FONT;
	let lines: ReturnType<typeof wrapText> = [];
	if(unit.text) {
		const prefixMatch = unit.text.match(/^(\d+\.\s)/);
		const indent = prefixMatch ?
			measureCtx.measureText(prefixMatch[1]).width : 0;
		lines = wrapText(measureCtx, unit.text, CELL_W - TEXT_PAD * 2, indent);
	}
	const textH = lines.length > 0 ? TEXT_PAD + lines.length * LINE_H + TEXT_PAD : 0;

	const canvas = document.createElement("canvas");
	canvas.width = CELL_W;
	canvas.height = DIAGRAM_PX + textH;
	const ctx = canvas.getContext("2d")!;

	if(!transparent) {
		ctx.fillStyle = "#ffffff";
		ctx.fillRect(0, 0, canvas.width, canvas.height);
	}

	ctx.drawImage(img, 0, 0, DIAGRAM_PX, DIAGRAM_PX);

	if(lines.length > 0) {
		ctx.font = FONT;
		ctx.fillStyle = theme === "light" ? "#212529" : getComputedStyle(document.body).color || "#212529";
		lines.forEach((line, i) => {
			ctx.fillText(line.text, TEXT_PAD + line.x, DIAGRAM_PX + TEXT_PAD + (i + 1) * LINE_H);
		});
	}

	return canvas;
}

function compositeGrid(cells: HTMLCanvasElement[], transparent: boolean): HTMLCanvasElement {
	const cols = Math.min(GRID_COLS, cells.length);
	const rows = Math.ceil(cells.length / cols);
	const cellH = Math.max(...cells.map(c => c.height));
	const w = cols * CELL_W + (cols - 1) * GAP_PX;
	const h = rows * cellH + (rows - 1) * GAP_PX;

	const canvas = document.createElement("canvas");
	canvas.width = w;
	canvas.height = h;
	const ctx = canvas.getContext("2d")!;

	if(!transparent) {
		ctx.fillStyle = "#ffffff";
		ctx.fillRect(0, 0, w, h);
	}

	cells.forEach((cell, i) => {
		const col = i % cols;
		const row = Math.floor(i / cols);
		ctx.drawImage(cell, col * (CELL_W + GAP_PX), row * (cellH + GAP_PX));
	});

	return canvas;
}

// ---------------------------------------------------------------------------
// PNG
// ---------------------------------------------------------------------------

export async function exportAsPng(cardBody: HTMLDivElement): Promise<Blob> {
	const steps = collectSteps(cardBody);
	const cells = await Promise.all(steps.map(u => stepToCanvas(u, true)));
	const canvas = compositeGrid(cells, true);
	return new Promise((resolve, reject) => {
		canvas.toBlob(
			b => b ? resolve(b) : reject(new Error("Canvas toBlob failed")),
			"image/png"
		);
	});
}

// ---------------------------------------------------------------------------
// SVG
// ---------------------------------------------------------------------------

const SVG_FONT_SIZE = 5;
const SVG_LINE_H = 6.5;
// Scale factor: map SVG units to canvas px for wrapText measurement
const SVG_MEASURE_SCALE = 3;

function buildSvgTextLines(unit: StepUnit, g: SVGGElement, NS: string): void {
	if(!unit.text) return;

	// Use a temporary canvas to measure text widths in "px" equivalent of SVG units
	const canvas = document.createElement("canvas");
	const ctx = canvas.getContext("2d")!;
	ctx.font = `${SVG_FONT_SIZE * SVG_MEASURE_SCALE}px sans-serif`;
	const maxWidthPx = CELL_SVG * SVG_MEASURE_SCALE;
	const prefixMatch = unit.text.match(/^(\d+\.\s)/);
	const indentPx = prefixMatch ? ctx.measureText(prefixMatch[1]).width : 0;
	const lines = wrapText(ctx, unit.text, maxWidthPx, indentPx);

	let lineY = CELL_SVG + SVG_FONT_SIZE + 1;
	for(const line of lines) {
		const t = document.createElementNS(NS, "text");
		t.setAttribute("x", String(line.x / SVG_MEASURE_SCALE));
		t.setAttribute("y", String(lineY));
		t.setAttribute("font-size", String(SVG_FONT_SIZE));
		t.setAttribute("font-family", "sans-serif");
		t.setAttribute("fill", "#212529");
		t.textContent = line.text;
		g.appendChild(t);
		lineY += SVG_LINE_H;
	}
}

export function exportAsSvg(cardBody: HTMLDivElement): Promise<Blob> {
	const NS = "http://www.w3.org/2000/svg";
	const steps = collectSteps(cardBody);
	const cols = Math.min(GRID_COLS, steps.length);
	const rows = Math.ceil(steps.length / cols);
	const hasText = steps.some(u => u.text);
	const cellSvgH = hasText ? CELL_SVG + 40 : CELL_SVG;
	const totalW = cols * CELL_SVG + (cols - 1) * GAP_SVG;
	const totalH = rows * cellSvgH + (rows - 1) * GAP_SVG;

	const root = document.createElementNS(NS, "svg");
	root.setAttribute("xmlns", NS);
	root.setAttribute("width", String(totalW));
	root.setAttribute("height", String(totalH));
	root.setAttribute("viewBox", `0 0 ${totalW} ${totalH}`);

	steps.forEach((unit, i) => {
		const col = i % cols;
		const row = Math.floor(i / cols);
		const x = col * (CELL_SVG + GAP_SVG);
		const y = row * (cellSvgH + GAP_SVG);

		const g = document.createElementNS(NS, "g");
		g.setAttribute("transform", `translate(${x}, ${y})`);

		const inlined = inlineStyles(unit.svg);
		inlined.setAttribute("width", String(CELL_SVG));
		inlined.setAttribute("height", String(CELL_SVG));
		g.appendChild(inlined);

		buildSvgTextLines(unit, g, NS);
		root.appendChild(g);
	});

	const serialized = new XMLSerializer().serializeToString(root);
	return Promise.resolve(new Blob([serialized], { type: "image/svg+xml" }));
}

// ---------------------------------------------------------------------------
// PDF
// ---------------------------------------------------------------------------

export async function exportAsPdf(cardBody: HTMLDivElement): Promise<Blob> {
	const { jsPDF } = await import("jspdf");

	const steps = collectSteps(cardBody);
	const cells = await Promise.all(steps.map(u => stepToCanvas(u, false, "light")));
	const grid = compositeGrid(cells, false);

	const PAGE_W = 210;
	const PAGE_H = 297;
	const MARGIN = 10;
	const availW = PAGE_W - MARGIN * 2;
	const availH = PAGE_H - MARGIN * 2;
	const ratio = Math.min(availW / grid.width, availH / grid.height);

	// eslint-disable-next-line new-cap
	const pdf = new jsPDF({
		orientation: "portrait",
		unit: "mm",
		format: "a4",
	});
	pdf.addImage(
		grid.toDataURL("image/png"), "PNG",
		MARGIN, MARGIN,
		grid.width * ratio, grid.height * ratio
	);

	return pdf.output("blob");
}

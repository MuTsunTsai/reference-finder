/** Crease types, following ORIPA's .cp format. */
export enum CreaseType {
	none = 0,
	border = 1,
	mountain = 2,
	valley = 3,
	auxiliary = 4,
}

export interface CpLine {
	type: CreaseType;
	p1: IPoint;
	p2: IPoint;
}

/** A crease pattern as read from the file, in the coordinates of the file. */
export interface CreasePattern {
	name: string;
	lines: CpLine[];
	/** Distinct endpoints of the lines. */
	vertices: IPoint[];
	/** Bounding box of the pattern. */
	box: Box;
}

interface Box {
	minX: number;
	maxY: number;
	width: number;
	height: number;
}

/** A crease pattern transformed onto the sheet, with the origin at the lower-left corner. */
export interface FittedPattern {
	lines: CpLine[];
	vertices: IPoint[];
	/** Whether the aspect ratio of the pattern differs from that of the sheet. */
	aspectMismatch: boolean;
}

export interface CpSelection {
	type: "point" | "line";
	/** Index in {@link FittedPattern.vertices} or {@link FittedPattern.lines}. */
	index: number;
}

const KEY_PRECISION = 9;
const ASPECT_TOLERANCE = 1e-6;

/**
 * Parses a .cp (ORIPA) or .fold file. The format is decided by the extension,
 * falling back to detecting JSON content.
 */
export function parseCp(name: string, content: string): CreasePattern {
	const isFold = /\.fold$/i.test(name) || !/\.cp$/i.test(name) && content.trimStart().startsWith("{");
	const lines = isFold ? parseFold(content) : parseOripa(content);
	if(lines.length == 0) throw new Error("No lines found");
	const vertices = collectVertices(lines);
	const box = getBoundingBox(vertices);
	if(!box.width || !box.height) throw new Error("The crease pattern has zero area");
	return { name, lines, vertices, box };
}

/** Each line goes like `type x1 y1 x2 y2`. */
function parseOripa(content: string): CpLine[] {
	const result: CpLine[] = [];
	for(const row of content.split(/\r?\n/)) {
		const text = row.trim();
		if(!text) continue;
		const values = text.split(/\s+/).map(Number);
		if(values.length < 5 || values.some(v => isNaN(v))) throw new Error("Invalid line: " + text);
		const [type, x1, y1, x2, y2] = values;
		result.push({ type: toCreaseType(type), p1: [x1, y1], p2: [x2, y2] });
	}
	return result;
}

interface FoldFrame {
	vertices_coords?: number[][];
	edges_vertices?: number[][];
	edges_assignment?: string[];
	file_frames?: FoldFrame[];
}

/** @see https://github.com/edemaine/fold */
function parseFold(content: string): CpLine[] {
	let fold = JSON.parse(content) as FoldFrame;
	if(!fold.vertices_coords && fold.file_frames?.length) fold = fold.file_frames[0];
	const coords = fold.vertices_coords;
	const edges = fold.edges_vertices;
	if(!coords || !edges) throw new Error("Missing vertices_coords or edges_vertices");
	return edges.map(([i, j], k) => {
		if(!coords[i] || !coords[j]) throw new Error("Invalid edge: " + k);
		return {
			type: fromAssignment(fold.edges_assignment?.[k]),
			p1: [coords[i][0], coords[i][1]],
			p2: [coords[j][0], coords[j][1]],
		};
	});
}

function toCreaseType(type: number): CreaseType {
	return type >= CreaseType.border && type <= CreaseType.valley ? type : CreaseType.auxiliary;
}

function fromAssignment(assignment: string | undefined): CreaseType {
	switch(assignment?.toUpperCase()) {
		case "B":
		case "C":
			return CreaseType.border;
		case "M":
			return CreaseType.mountain;
		case "V":
			return CreaseType.valley;
		default:
			return CreaseType.auxiliary;
	}
}

function collectVertices(lines: CpLine[]): IPoint[] {
	const map = new Map<string, IPoint>();
	for(const { p1, p2 } of lines) {
		for(const p of [p1, p2]) {
			const key = p[0].toFixed(KEY_PRECISION) + "," + p[1].toFixed(KEY_PRECISION);
			if(!map.has(key)) map.set(key, p);
		}
	}
	return [...map.values()];
}

/**
 * Maps the bounding box of the pattern onto the sheet.
 * Both .cp and .fold files (as exported by ORIPA, BP Studio, etc.) use screen coordinates
 * with the y-axis pointing down, so the y-axis is flipped here.
 */
export function fitToSheet(cp: CreasePattern, width: number, height: number): FittedPattern {
	const { minX, maxY, width: w, height: h } = cp.box;
	const map = (p: IPoint): IPoint => [(p[0] - minX) / w * width, (maxY - p[1]) / h * height];
	return {
		lines: cp.lines.map(l => ({ type: l.type, p1: map(l.p1), p2: map(l.p2) })),
		vertices: cp.vertices.map(map),
		aspectMismatch: !matchesAspectRatio(cp, width, height),
	};
}

/** Whether the aspect ratio of the pattern is the same as that of the given sheet. */
export function matchesAspectRatio(cp: CreasePattern, width: number, height: number): boolean {
	const ratio = width / height;
	return Math.abs(getAspectRatio(cp) - ratio) <= ASPECT_TOLERANCE * ratio;
}

/** The width-to-height ratio of the pattern. */
export function getAspectRatio(cp: CreasePattern): number {
	return cp.box.width / cp.box.height;
}

function getBoundingBox(vertices: IPoint[]): Box {
	// Not using Math.min(...xs), as large patterns could exceed the argument limit
	let minX = Infinity, maxX = -Infinity, minY = Infinity, maxY = -Infinity;
	for(const [x, y] of vertices) {
		minX = Math.min(minX, x);
		maxX = Math.max(maxX, x);
		minY = Math.min(minY, y);
		maxY = Math.max(maxY, y);
	}
	return { minX, maxY, width: maxX - minX, height: maxY - minY };
}

/** Returns the points defining the selection on the sheet. */
export function getSelectionPoints(fitted: FittedPattern, selection: CpSelection): IPoint[] {
	if(selection.type == "point") return [fitted.vertices[selection.index]];
	const line = fitted.lines[selection.index];
	return [line.p1, line.p2];
}

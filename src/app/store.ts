import { create } from "zustand";
import { persist } from "zustand/middleware";

export interface Solution {
	solution: IPoint | [number, IPoint];
	err: number;
	rank: number;
	steps: Step[];
	diagrams: DiagramElement[][];
}

export type RefElement = PointElement | LineElement;
export type DiagramElement = RefElement | ArcElement | PolyElement | LabelElement | null;

export interface StepBase {
	axiom: number;
	p0?: string;
	p1?: string;
	l0?: string;
	l1?: string;
	x: string;
	order?: string;

	/** Generated in UI, if {@link order} is given. */
	n?: string[];
}

export interface Step extends StepBase {
	intersection?: StepBase;
	pinch?: boolean;
}

export enum Theme {
	system,
	light,
	dark,
	rabbit,
};

export const defaultSettings = {
	showInit: true,
	showAxiom: false,
	useDB: true,
	theme: Theme.system,
	error: 0.005,
	errorExp: "1/200",
	worstCaseError: true,
	count: 5,
	precision: 4,
	trials: 100000,
};

export interface InitInfo {
	rank: number;
	lines: number;
	marks: number;
}

const defaultStore = {
	progress: null as InitInfo | null,
	ready: false,
	running: false,
	coreError: null as string | null,
	solutions: [] as Solution[],
	statisticsRunning: false,
	computedTheme: Theme.light,
	existingRefs: [] as RefElement[], // for rendering on the preview paper
	existingMarks: [] as IPoint[], // for computing new solutions from previous ones
	existingLines: [] as ISegment[], // for computing new solutions from previous ones
};

export const defaultDB = {
	width: 1,
	widthExp: "1",
	height: 1,
	heightExp: "1",
	maxRank: 6,
	maxLinesV1: 600000,
	maxMarksV1: 600000,
	useDivision: true,
	numX: 5000,
	numY: 5000,
	numA: 5000,
	numD: 5000,
	minAspectRatio: 0.1,
	minAspectRatioExp: "1/10",
	minAngleSine: 0.342,
	minAngleSineExp: "sin(20)",
	visibility: true,
	axioms: [true, true, true, true, true, true, true],
	axiomPriority: [2, 3, 7, 6, 5, 4, 1], // The default priority
};
export type DbSettings = typeof defaultDB;

// localStorage is shared by all apps on mutsuntsai.github.io, so the keys must be prefixed.
// (SETTINGS_KEY is also read by an inline script in public/index.html.)
export const SETTINGS_KEY = "reference-finder:settings";
const DB_KEY = "reference-finder:db";

/**
 * Moves the persisted stores from the old unprefixed keys, which collide with other apps on the same origin
 * (in particular, FEN Tool also used "settings").
 *
 * TODO: Migration code; can be removed after 2027-10.
 */
function migrateStorage(): void {
	for(const [oldKey, newKey] of [["settings", SETTINGS_KEY], ["db", DB_KEY]]) {
		try {
			const value = localStorage.getItem(oldKey);
			if(value === null || localStorage.getItem(newKey) !== null) continue;
			const data = JSON.parse(value);
			// Only take the value if it's in the shape persisted by zustand; otherwise it belongs to another app.
			if(data && typeof data == "object" && "state" in data && "version" in data) {
				localStorage.setItem(newKey, value);
				localStorage.removeItem(oldKey);
			}
		} catch {
			// Ignore broken data or unavailable storage
		}
	}
}
migrateStorage();

export const useStore = create(() => defaultStore);
export const useSettings = create(persist(() => defaultSettings, { name: SETTINGS_KEY }));
export const useDB = create(persist(() => defaultDB, { name: DB_KEY }));

export enum ElementType {
	point,
	line,
	arc,
	poly,
	label,
}

export interface PointElement {
	type: ElementType.point;
	pt: IPoint;
	style: PointStyle;
}

export enum PointStyle {
	normal,
	highlight,
	action,
};

export interface LineElement {
	type: ElementType.line;
	from: IPoint;
	to: IPoint;
	style: LineStyle;
}

export enum LineStyle {
	crease,
	edge,
	highlight,
	valley,
	mountain,
	arrow,
	dotted,
	pinch,
};

export interface ArcElement {
	type: ElementType.arc;
	center: IPoint;
	radius: number;
	from: number;
	to: number;
	ccw: number;
	style: LineStyle;
}

export interface PolyElement {
	type: ElementType.poly;
	width: number;
	height: number;
}

export interface LabelElement {
	type: ElementType.label;
	pt: IPoint;
	text: string;
	style: LabelStyle;
}

export enum LabelStyle {
	normal,
	highlight,
	action,
};

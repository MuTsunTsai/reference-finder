import type { DbSettings } from "./store";

/** Distance from point p to the infinite line through a and b. */
function distanceToLine(p: IPoint, a: IPoint, b: IPoint): number {
	const dx = b[0] - a[0];
	const dy = b[1] - a[1];
	return Math.abs(dx * (p[1] - a[1]) - dy * (p[0] - a[0])) / Math.hypot(dx, dy);
}

/**
 * Additional lines (other than the edges) that are passed to the core as existing lines
 * when marks are restricted, as the core only allows marks on the edges and the existing lines.
 */
export function getRestrictionLines(db: DbSettings): ISegment[] {
	const lines: ISegment[] = [];
	if(!db.restrictMarks) return lines;
	const { width: w, height: h } = db;
	if(db.allowDiagonals) lines.push([[0, 0], [w, h]], [[0, h], [w, 0]]);
	if(db.allowHorizontalFold) lines.push([[0, h / 2], [w, h / 2]]);
	if(db.allowVerticalFold) lines.push([[w / 2, 0], [w / 2, h]]);
	return lines;
}

/**
 * Checks if the target point can be found under the mark restriction,
 * i.e. it lies (within the tolerance) on an edge, a restriction line or an existing line.
 * Must be consistent with ReferenceFinder::ValidateMark in the core.
 */
export function isOnPinchLine(p: IPoint, db: DbSettings, existingLines: ISegment[], tolerance: number): boolean {
	if(!db.restrictMarks) return true;
	const { width: w, height: h } = db;
	const lines: ISegment[] = [
		[[0, 0], [w, 0]],
		[[0, 0], [0, h]],
		[[w, 0], [w, h]],
		[[0, h], [w, h]],
		...getRestrictionLines(db),
		...existingLines,
	];
	return lines.some(([a, b]) => distanceToLine(p, a, b) <= tolerance);
}

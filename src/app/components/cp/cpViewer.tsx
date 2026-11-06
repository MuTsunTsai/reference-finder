import { useCallback, useEffect, useMemo, useRef, useState } from "react";

import { CreaseType } from "../../cp/parse";

import type { CpSelection, FittedPattern } from "../../cp/parse";

import "./cpViewer.scss";

interface CpViewerProps {
	cp: FittedPattern;
	width: number;
	height: number;
	selection: CpSelection | null;
	/** Changing this value fits the pattern into the view again. */
	fitKey: number;
	onSelect: (selection: CpSelection | null) => void;
	onHover?: (selection: CpSelection | null) => void;
}

/**
 * Maps sheet coordinates to screen coordinates
 * by `(tx + x * scale, ty - y * scale)`.
 */
interface View {
	scale: number;
	tx: number;
	ty: number;
}

interface Gesture {
	view: View;
	/**
	 * Whether the pointer has moved beyond {@link CLICK_SLOP},
	 * so it's not a click.
	 */
	moved: boolean;
	start: IPoint;
	/** For two-finger gestures. */
	distance?: number;
	center?: IPoint;
}

const PADDING = 24;
/**
 * Hit radius in pixels; points are given a larger radius
 * so that they're easier to select.
 */
const POINT_HIT = 12;
const LINE_HIT = 6;
const CLICK_SLOP = 4;
const MAX_ZOOM = 500;
const DOT_RADIUS = 5;
const WHEEL_FACTOR = 0.0015;
const MIDDLE_BUTTON = 1;
const FIT_TOLERANCE = 1e-6;

const CLASS_NAMES: Record<CreaseType, string> = {
	[CreaseType.none]: "cp-aux",
	[CreaseType.border]: "cp-border",
	[CreaseType.mountain]: "cp-mountain",
	[CreaseType.valley]: "cp-valley",
	[CreaseType.auxiliary]: "cp-aux",
};

function distanceToSegment(p: IPoint, a: IPoint, b: IPoint): number {
	const dx = b[0] - a[0], dy = b[1] - a[1];
	const l2 = dx * dx + dy * dy;
	const dot = (p[0] - a[0]) * dx + (p[1] - a[1]) * dy;
	const t = l2 ? Math.max(0, Math.min(1, dot / l2)) : 0;
	return Math.hypot(p[0] - a[0] - t * dx, p[1] - a[1] - t * dy);
}

function getCenter(pts: IPoint[]): IPoint {
	return [(pts[0][0] + pts[1][0]) / 2, (pts[0][1] + pts[1][1]) / 2];
}

function getDistance(pts: IPoint[]): number {
	return Math.hypot(pts[0][0] - pts[1][0], pts[0][1] - pts[1][1]);
}

/** Zooms the view by factor k around the screen point c. */
function zoomAround(view: View, k: number, c: IPoint): View {
	return {
		scale: view.scale * k,
		tx: c[0] - (c[0] - view.tx) * k,
		ty: c[1] - (c[1] - view.ty) * k,
	};
}

/**
 * Displays a crease pattern, which can be zoomed
 * (by the mouse wheel or pinching) and panned (by dragging),
 * and from which a point or a line can be selected.
 */
export function CpViewer({
	cp, width, height, selection, fitKey, onSelect, onHover,
}: CpViewerProps) {
	const ref = useRef<HTMLDivElement>(null);
	const [view, setView] = useState<View>({ scale: 1, tx: 0, ty: 0 });
	const [hover, setHover] = useState<CpSelection | null>(null);
	/**
	 * The view where the whole sheet fits;
	 * this is also the most zoomed-out view.
	 */
	const fitView = useRef<View>({ scale: 1, tx: 0, ty: 0 });
	const pointers = useRef(new Map<number, IPoint>());
	const gesture = useRef<Gesture | null>(null);

	const fit = useCallback(() => {
		const el = ref.current;
		if(!el || !el.clientWidth || !el.clientHeight) return;
		const w = el.clientWidth, h = el.clientHeight;
		const scale = Math.min(
			(w - 2 * PADDING) / width,
			(h - 2 * PADDING) / height
		);
		fitView.current = {
			scale,
			tx: (w - width * scale) / 2,
			ty: (h + height * scale) / 2,
		};
		setView(fitView.current);
	}, [width, height]);

	useEffect(fit, [fit, fitKey]);

	/**
	 * Zooms by factor k around the screen point c, within the allowed range.
	 */
	const zoom = useCallback((v: View, k: number, c: IPoint): View => {
		const min = fitView.current.scale, max = min * MAX_ZOOM;
		k = Math.min(max / v.scale, k);
		// When zoomed out all the way, go back to the fitted view
		if(v.scale * k <= min * (1 + FIT_TOLERANCE)) return fitView.current;
		return zoomAround(v, k, c);
	}, []);

	// React registers wheel listeners as passive,
	// so we have to add it manually to prevent scrolling
	useEffect(() => {
		const el = ref.current;
		if(!el) return;
		const handler = (e: WheelEvent) => {
			e.preventDefault();
			const rect = el.getBoundingClientRect();
			const c: IPoint = [e.clientX - rect.left, e.clientY - rect.top];
			setView(v => zoom(v, Math.exp(-e.deltaY * WHEEL_FACTOR), c));
		};
		el.addEventListener("wheel", handler, { passive: false });
		return () => el.removeEventListener("wheel", handler);
	}, [zoom]);

	function toScreen(e: React.PointerEvent): IPoint {
		const rect = ref.current!.getBoundingClientRect();
		return [e.clientX - rect.left, e.clientY - rect.top];
	}

	function hitTest(s: IPoint): CpSelection | null {
		const p: IPoint = [
			(s[0] - view.tx) / view.scale,
			(view.ty - s[1]) / view.scale,
		];
		let best = POINT_HIT / view.scale, index = -1;
		cp.vertices.forEach((v, i) => {
			const d = Math.hypot(v[0] - p[0], v[1] - p[1]);
			if(d < best) [best, index] = [d, i];
		});
		if(index >= 0) return { type: "point", index };
		best = LINE_HIT / view.scale;
		cp.lines.forEach((l, i) => {
			const d = distanceToSegment(p, l.p1, l.p2);
			if(d < best) [best, index] = [d, i];
		});
		return index >= 0 ? { type: "line", index } : null;
	}

	function updateHover(h: CpSelection | null) {
		if(h?.type === hover?.type && h?.index === hover?.index) return;
		setHover(h);
		onHover?.(h);
	}

	function startGesture() {
		const pts = [...pointers.current.values()];
		gesture.current = pts.length == 2 ?
			{
				view,
				moved: true,
				start: pts[0],
				distance: getDistance(pts),
				center: getCenter(pts),
			} :
			// If a finger is lifted from a two-finger gesture,
			// the remaining one should not cause a selection
			{ view, moved: gesture.current?.moved ?? false, start: pts[0] };
	}

	function pointerDown(e: React.PointerEvent) {
		if(e.button > MIDDLE_BUTTON) return;
		try {
			e.currentTarget.setPointerCapture(e.pointerId);
		} catch {
			// The pointer may already be released;
			// the gesture still works without capturing
		}
		pointers.current.set(e.pointerId, toScreen(e));
		if(pointers.current.size == 1) gesture.current = null;
		if(pointers.current.size <= 2) startGesture();
		// Dragging with the middle button always pans, and never selects
		if(e.button == MIDDLE_BUTTON && gesture.current) {
			gesture.current.moved = true;
		}
	}

	function pointerMove(e: React.PointerEvent) {
		const s = toScreen(e);
		const g = gesture.current;
		if(!pointers.current.has(e.pointerId) || !g) {
			if(e.pointerType == "mouse") updateHover(hitTest(s));
			return;
		}
		pointers.current.set(e.pointerId, s);
		const pts = [...pointers.current.values()];
		if(pts.length == 1) {
			const dx = s[0] - g.start[0], dy = s[1] - g.start[1];
			if(Math.hypot(dx, dy) > CLICK_SLOP) g.moved = true;
			if(g.moved) {
				setView({ ...g.view, tx: g.view.tx + dx, ty: g.view.ty + dy });
			}
		} else if(pts.length == 2 && g.distance && g.center) {
			const center = getCenter(pts);
			const v = zoom(g.view, getDistance(pts) / g.distance, g.center);
			if(v == fitView.current) {
				setView(v);
			} else {
				// Keep the sheet point under the original center
				// under the new center
				setView({
					...v,
					tx: v.tx + center[0] - g.center[0],
					ty: v.ty + center[1] - g.center[1],
				});
			}
		}
	}

	function pointerUp(e: React.PointerEvent) {
		if(!pointers.current.has(e.pointerId)) return;
		const g = gesture.current;
		if(
			pointers.current.size == 1 && g && !g.moved &&
			e.type == "pointerup"
		) {
			onSelect(hitTest(toScreen(e)));
		}
		pointers.current.delete(e.pointerId);
		if(pointers.current.size == 0) gesture.current = null;
		else startGesture();
	}

	// The lines are memoized,
	// so that they're not re-rendered during zooming and panning
	const lines = useMemo(() => cp.lines.map((l, i) => (
		<line
			key={i}
			x1={l.p1[0]}
			y1={l.p1[1]}
			x2={l.p2[0]}
			y2={l.p2[1]}
			className={CLASS_NAMES[l.type]}
		/>
	)), [cp]);

	function renderMark(s: CpSelection | null, className: string) {
		if(!s) return null;
		if(s.type == "point") {
			const [x, y] = cp.vertices[s.index];
			return (
				<circle
					cx={x}
					cy={y}
					r={DOT_RADIUS / view.scale}
					className={className}
				/>
			);
		}
		const { p1, p2 } = cp.lines[s.index];
		return (
			<line
				x1={p1[0]}
				y1={p1[1]}
				x2={p2[0]}
				y2={p2[1]}
				className={className}
			/>
		);
	}

	const isSelected = hover && selection &&
		hover.type == selection.type && hover.index == selection.index;

	return (
		<div className="cp-viewer" ref={ref}>
			<svg
				className={hover ? "cp-hovering" : ""}
				onPointerDown={pointerDown}
				onPointerMove={pointerMove}
				onPointerUp={pointerUp}
				onPointerCancel={pointerUp}
				onPointerLeave={() => updateHover(null)}
				// Prevent the auto-scrolling of the browser
				onMouseDown={
					e => e.button == MIDDLE_BUTTON && e.preventDefault()
				}
			>
				<g transform={`matrix(${view.scale} 0 0 ${-view.scale} ${view.tx} ${view.ty})`}>
					<rect x={0} y={0} width={width} height={height} className="cp-sheet" />
					{lines}
					{!isSelected && renderMark(hover, "cp-hover")}
					{renderMark(selection, "cp-selected")}
				</g>
			</svg>
		</div>
	);
}

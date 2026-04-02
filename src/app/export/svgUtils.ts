const PROPS = [
	"fill",
	"stroke",
	"stroke-width",
	"stroke-dasharray",
	"stroke-linecap",
	"stroke-linejoin",
	"paint-order",
	"marker-start",
	"marker-end",
	"font-size",
	"text-anchor",
	"display",
] as const;

/**
 * Creates (or reuses) a hidden off-screen container with the given data-bs-theme,
 * appends a clone of the SVG into it to force style resolution, reads computed styles,
 * then removes the container. Returns the styled clone (detached from DOM).
 */
function resolveComputedStyles(svg: SVGSVGElement, theme: string): SVGSVGElement {
	const container = document.createElement("div");
	container.style.cssText = "position:absolute;left:-9999px;top:-9999px;visibility:hidden;pointer-events:none;";

	// Wrap in a fake #root so that theme CSS rules (scoped to [data-bs-theme="..."] #root) apply correctly
	container.dataset.bsTheme = theme;
	const fakeRoot = document.createElement("div");
	if(theme != "light") fakeRoot.id = "root";
	container.appendChild(fakeRoot);

	const clone = svg.cloneNode(true) as SVGSVGElement;
	// Mirror into the fake root so computed styles resolve correctly
	const liveCopy = svg.cloneNode(true) as SVGSVGElement;
	fakeRoot.appendChild(liveCopy);

	document.body.appendChild(container);

	try {
		const liveEls = Array.from(liveCopy.querySelectorAll<SVGElement>("*"));
		const cloneEls = Array.from(clone.querySelectorAll<SVGElement>("*"));

		liveEls.forEach((el, i) => {
			const computed = getComputedStyle(el);
			const target = cloneEls[i];
			for(const prop of PROPS) {
				const val = computed.getPropertyValue(prop);
				if(val) target.style.setProperty(prop, val);
			}
			target.removeAttribute("class");
		});
	} finally {
		document.body.removeChild(container);
	}

	return clone;
}

/**
 * Inlines computed styles of an SVG element so it renders correctly as a standalone file.
 * Returns a deep clone; the original DOM is not modified and the page never visually flickers.
 *
 * @param svg    The live SVG element in the document.
 * @param theme  The data-bs-theme value to use when resolving styles. Defaults to the current theme.
 */
export function inlineStyles(svg: SVGSVGElement, theme?: "light"): SVGSVGElement {
	const resolvedTheme = theme ?? document.documentElement.dataset.bsTheme ?? "light";
	const clone = resolveComputedStyles(svg, resolvedTheme);
	inlineMarkers(clone, resolvedTheme);
	return clone;
}

/**
 * Collects all marker IDs referenced via url(#...) in marker-start / marker-end style properties.
 */
function collectMarkerIds(clone: SVGSVGElement): Set<string> {
	// Match both url(#id) and url("...#id") - browsers expand relative urls to absolute in computed styles
	const urlRe = /url\([^)]*#([^)"]+)[")]/g;
	const ids = new Set<string>();
	for(const el of Array.from(clone.querySelectorAll<SVGElement>("*"))) {
		for(const attr of ["marker-start", "marker-end"] as const) {
			const val = el.style.getPropertyValue(attr) || el.getAttribute(attr) || "";
			let m: RegExpExecArray | null;
			urlRe.lastIndex = 0;
			while((m = urlRe.exec(val)) !== null) ids.add(m[1]);
		}
	}
	return ids;
}

/**
 * Clones a live marker element into the SVG's <defs>, inlines fill on its children,
 * and rewrites all references in the clone to use the new unique ID.
 */
function inlineOneMarker(clone: SVGSVGElement, defs: Element, id: string, suffix: string, theme: string): void {
	const liveMarker = document.getElementById(id);
	if(!liveMarker) return;
	const markerClone = liveMarker.cloneNode(true) as Element;
	const newId = id + suffix;
	markerClone.setAttribute("id", newId);

	// Resolve fill color under the correct theme using an off-screen container
	const container = document.createElement("div");
	container.dataset.bsTheme = theme;
	container.style.cssText = "position:absolute;left:-9999px;top:-9999px;visibility:hidden;pointer-events:none;";
	const fakeRoot = document.createElement("div");
	if(theme != "light") fakeRoot.id = "root";
	container.appendChild(fakeRoot);
	const markerCopy = liveMarker.cloneNode(true) as Element;
	fakeRoot.appendChild(markerCopy);
	document.body.appendChild(container);
	try {
		for(const path of Array.from(markerClone.querySelectorAll<SVGElement>("path, polygon"))) {
			const live = markerCopy.querySelector(path.tagName);
			if(live) {
				const fill = getComputedStyle(live).getPropertyValue("fill");
				if(fill) path.style.fill = fill;
			}
		}
	} finally {
		document.body.removeChild(container);
	}
	defs.appendChild(markerClone);

	const newRef = `url(#${newId})`;
	// Match both url(#id) and url("#id") forms
	const refRe = new RegExp(`url\\(["']?#${id}["']?\\)`, "g");
	for(const el of Array.from(clone.querySelectorAll<SVGElement>("*"))) {
		for(const attr of ["marker-start", "marker-end"] as const) {
			const val = el.style.getPropertyValue(attr);
			if(refRe.test(val)) {
				refRe.lastIndex = 0;
				el.style.setProperty(attr, val.replace(refRe, newRef));
			}
		}
	}
}

/**
 * Finds all marker references (marker-start / marker-end with url(#...)) in the clone,
 * copies the corresponding <marker> elements from the live document into a <defs> block,
 * and rewrites the references to use unique IDs to avoid conflicts when multiple SVGs are combined.
 */
function inlineMarkers(clone: SVGSVGElement, theme: string): void {
	const ids = collectMarkerIds(clone);
	if(ids.size === 0) return;

	const NS = "http://www.w3.org/2000/svg";
	const suffix = `-export-${Math.random().toString(36).slice(2, 7)}`;

	let defs = clone.querySelector("defs");
	if(!defs) {
		defs = document.createElementNS(NS, "defs");
		clone.prepend(defs);
	}

	for(const id of Array.from(ids)) inlineOneMarker(clone, defs, id, suffix, theme);
}

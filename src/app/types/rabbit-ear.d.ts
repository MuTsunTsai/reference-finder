declare module "rabbit-ear" {

	export function svg(): RabbitEarSVG;

	export const cp: {
		(): Fold;
		rectangle: (width?: number, height?: number) => Fold;
		square: () => Fold;
	}

	export const axiom: {
		axiom1(p1: IPoint, p2: IPoint): RayLine;
	}

	export const line: {
		fromPoints(p1: IPoint, p2: IPoint): RayLine;
	}

	export function rect(x: number, y: number, width: number, height: number): Rect;

	interface Rect {
		clip(line: RayLine): Segment;
	}

	interface RayLine {
		vector: IPoint;
		origin: IPoint;
	}

	type ISegment = [IPoint, IPoint];
	interface Segment extends ISegment, RayLine { }

	interface RabbitEarOrigami extends RabbitEarSVG {
		readonly vertices: RabbitEarSVG;
		readonly edges: RabbitEarSVG;
		readonly boundary: RabbitEarSVG;
		readonly faces: RabbitEarSVG;
	}

	interface RabbitEarSVG extends SVGElement {
		origami(fold: Fold): RabbitEarOrigami;
		size(x: number, y: number, width?: number, height?: number): this;

		circle(origin: IPoint | number, radius: number): RabbitEarSVG;
		line(p1: IPoint, p2: IPoint): RabbitEarSVG;
		arc(x: number, y: number, radius: number, fromAngle: number, toAngle: number, includeCenter?: boolean): RabbitEarSVG;
		text(text: string, p: IPoint): RabbitEarSVG;

		// Auto methods
		fill(value: string): this;
		padding(padding: number): this;
		stroke(color: string): this;
		strokeWidth(width: number): this;
	}
}

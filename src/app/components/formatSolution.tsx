
import type { Solution } from "../store";

/**
 * Calculates the degree using the normal vector of a line.
 */
export function getDegree(p: IPoint, precision: number): string {
	let result = Math.atan2(-p[0], p[1]) / Math.PI * 180;
	if(result <= -90) result += 180;
	if(result > 90) result -= 180;
	return result.toFixed(precision) + "°";
}

/**
 * Original ReferenceFinder represents a line as (distance to origin, normal unit vector),
 * and I think using (distance to origin, degree of the line) is more intuitive.
 */
export function formatSolution(data: Solution, precision: number): string {
	const item = data.solution[1];
	const text = typeof item == "number" ? item.toFixed(precision) : getDegree(item, precision);
	return `(${data.solution[0].toFixed(precision)}, ${text})`;
}


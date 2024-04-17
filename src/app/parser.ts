import MathExp from "math-expression-evaluator";

const mExp = new MathExp();

/**
 * I've tried a few options and math-expression-evaluator seems to be the best expression library.
 * Alternatives are:
 * - mathjs: too many unneeded features and thus too heavy
 * - expr-eval: not actively maintained, and inconvenient to use degree units
 * - hyoka: good otherwise but doesn't have `sqrt` function
 */
export function parse(v: string): number {
	try {
		// math-expression-evaluator use `root` instead of `sqrt`
		v = v.replace(/sqrt/g, "root");
		return mExp.eval(v);
	} catch {
		return NaN;
	}
}

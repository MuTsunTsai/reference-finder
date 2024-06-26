import MathExp from "math-expression-evaluator";

const mExp = new MathExp();

// math-expression-evaluator use `root` instead of `sqrt`,
// so we add this custom token to support the latter.
mExp.addToken([{
	type: 0, // FUNCTION_WITH_ONE_ARG
	token: "sqrt",
	show: "sqrt",
	value: Math.sqrt,
	precedence: 11, // same as other built-in functions
}]);

/**
 * I've tried a few options and math-expression-evaluator seems to be the best expression library.
 * Alternatives are:
 * - mathjs: too many unneeded features and thus too heavy
 * - expr-eval: not actively maintained, and inconvenient to use degree units
 * - hyoka: good otherwise but doesn't have `sqrt` function
 */
export function parse(v: string): number {
	try {
		return mExp.eval(v);
	} catch {
		return NaN;
	}
}

import pluginStyle from "@stylistic/eslint-plugin";

// Severities are all "error" in presets.
// We change those to "warn" here.
const preset = pluginStyle.configs["recommended-flat"];
for(const key in preset.rules) {
	const rule = preset.rules[key];
	if(rule == "error") preset.rules[key] = "warn";
	else preset.rules[key][0] = "warn";
}

export default [
	preset,
	{
		rules: {
			"@stylistic/array-bracket-newline": ["warn", "consistent"],
			"@stylistic/arrow-parens": ["warn", "as-needed"],
			"@stylistic/brace-style": [
				"warn",
				"1tbs",
				{
					allowSingleLine: true,
				},
			],
			"@stylistic/comma-dangle": [
				"warn",
				{
					arrays: "always-multiline",
					enums: "always-multiline",
					exports: "never",
					functions: "never",
					imports: "never",
					objects: "always-multiline",
				},
			],
			"@stylistic/generator-star-spacing": [
				"warn",
				{
					after: true,
					anonymous: "neither",
					before: false,
					method: {
						after: false,
						before: true,
					},
				},
			],
			"@stylistic/indent": ["warn", "tab"],
			"@stylistic/indent-binary-ops": "off",
			"@stylistic/jsx-closing-tag-location": "off",
			"@stylistic/jsx-curly-newline": "off",
			"@stylistic/jsx-indent": ["warn", "tab"],
			"@stylistic/jsx-indent-props": ["warn", "tab"],
			"@stylistic/jsx-one-expression-per-line": "off",
			"@stylistic/jsx-wrap-multilines": "off",
			"@stylistic/key-spacing": [
				"warn",
				{
					afterColon: true,
					mode: "strict",
				},
			],
			"@stylistic/keyword-spacing": "off",
			"@stylistic/max-len": [
				"warn",
				{
					code: 120,
					ignoreComments: true,
					ignoreRegExpLiterals: true,
					ignoreStrings: true,
					ignoreTemplateLiterals: true,
					tabWidth: 4,
				},
			],
			"@stylistic/max-statements-per-line": [
				"warn",
				{
					max: 2,
				},
			],
			"@stylistic/member-delimiter-style": [
				"warn",
				{
					singleline: {
						delimiter: "comma",
						requireLast: false,
					},
				},
			],
			"@stylistic/no-mixed-operators": "off",
			"@stylistic/no-mixed-spaces-and-tabs": ["warn", "smart-tabs"],
			"@stylistic/no-tabs": "off",
			"@stylistic/operator-linebreak": ["warn", "after"],
			"@stylistic/quote-props": ["warn", "as-needed"],
			"@stylistic/quotes": [
				"warn",
				"double",
				{
					allowTemplateLiterals: true,
					avoidEscape: true,
				},
			],
			"@stylistic/semi": ["warn", "always"],
			"@stylistic/space-before-function-paren": [
				"warn",
				{
					anonymous: "never",
					asyncArrow: "always",
					named: "never",
				},
			],
			"@stylistic/type-annotation-spacing": [
				"warn",
				{
					after: true,
					before: false,
					overrides: {
						arrow: {
							before: true,
						},
					},
				},
			],
			"@stylistic/wrap-iife": ["warn", "inside"],
		},
	},
];

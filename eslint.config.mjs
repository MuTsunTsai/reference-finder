import globals from "globals";
import pluginJs from "@eslint/js";
import pluginTs from "typescript-eslint";
import stylistic from "./config/stylistic.mjs";

export default [
	{
		ignores: [
			"docs/**/*",
			"node_modules/**/*",
			"src/lib/**/*",
		],
	},
	pluginJs.configs.recommended,
	...pluginTs.configs.recommended,
	...stylistic,
	{
		languageOptions: {
			globals: globals.browser,
		},
		rules: {
			"prefer-const": "warn",
		},
	},
	{
		files: ["*.js"],
		languageOptions: {
			globals: globals.node,
		},
	},
	{
		files: ["gulpfile.js"],
		rules: {
			"@typescript-eslint/no-var-requires": "off",
		},
	},
	{
		rules: {
			"@typescript-eslint/no-unused-vars": [
				"warn",
				{
					args: "none",
				},
			],
		},
	},
];

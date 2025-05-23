import pluginReact from "eslint-plugin-react";
import { createConfig } from "@mutsuntsai/eslint";

export default [
	...createConfig({
		ignores: [
			"docs/**",
			"node_modules/**",
			"src/lib/**",
		],
		import: ["**/*.{ts,tsx}", "eslint.config.mjs"],
		project: ["src/app"],
		globals: {
			cjs: ["gulpfile.js"],
			browser: ["src/**"],
		},
	}),
	{
		name: "TypeScript override",
		rules: {
			"@typescript-eslint/no-magic-numbers": "off", // Too many for this project
			"@typescript-eslint/no-unused-vars": [
				"warn",
				{
					args: "none",
				},
			],
		},
	},

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// React
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	{
		...pluginReact.configs.flat.all,
		...pluginReact.configs.flat["jsx-runtime"],
		files: ["**/*.tsx"],
		name: "React",
		plugins: {
			react: pluginReact,
		},
		rules: {
			"max-lines-per-function": "off",
			"new-cap": "off",
		},
	},

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Specific scopes
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	{
		name: "Gulp",
		files: ["gulpfile.js"],
		rules: {
			"@typescript-eslint/no-var-requires": "off",
			"@typescript-eslint/no-require-imports": "off",
		},
	},
];

import pluginReact from "eslint-plugin-react";
import { createConfig } from "@mutsuntsai/eslint";

export default [
	...createConfig({
		ignores: [
			"docs/**",
			"node_modules/**",
			"src/lib/**",
		],
		import: {
			files: ["**/*.{ts,tsx}", "eslint.config.js", "gulpfile.js"],
			project: ["src/app"],
		},
		globals: {
			esm: ["./*.{js,ts}"],
			browser: ["src/**"],
		},
	}),
	{
		name: "TypeScript override",
		files: ["**/*.{ts,tsx}"],
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

];

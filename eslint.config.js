import pluginReact from "eslint-plugin-react";
import { createConfig } from "@mutsuntsai/eslint";

export default [
	...createConfig({
		ignores: [
			"dist/**",
			"node_modules/**",
			"src/lib/**",
		],
		import: {
			files: ["**/*.{ts,tsx}", "eslint.config.js", "script/**", "gulpfile.js"],
			project: ["src/app"],
		},
		globals: {
			esm: ["./*.{js,ts}", "script/**"],
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
	{
		// The inline scripts must stay within ES2018,
		// so optional catch binding (`catch {}`) is not available
		name: "Inline script override",
		files: ["src/public/**/*.html"],
		rules: {
			"no-unused-vars": [
				"error",
				{
					caughtErrors: "none",
				},
			],
		},
	},

	////////////////////////////////////////////////////////////////////////
	// React
	////////////////////////////////////////////////////////////////////////

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

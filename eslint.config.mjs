import globals from "globals";
import pluginJs from "@eslint/js";
import pluginTs from "typescript-eslint";
import pluginImport from "eslint-plugin-import";
import pluginReact from "eslint-plugin-react";
import stylistic from "@mutsuntsai/stylistic";

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
		name: "General:TypeScript",
		files: ["**/*.{ts,tsx}"],
		rules: {
			"@typescript-eslint/class-methods-use-this": ["warn", {
				ignoreOverrideMethods: true,
				ignoreClassesThatImplementAnInterface: "public-fields",
			}],
			"@typescript-eslint/no-empty-object-type": ["warn", { allowInterfaces: "always" }],
			"@typescript-eslint/no-explicit-any": ["warn", { ignoreRestArgs: true }],
			"@typescript-eslint/no-invalid-this": "error",
			"@typescript-eslint/no-loop-func": ["warn"],
			"@typescript-eslint/no-shadow": "warn",
			"@typescript-eslint/no-this-alias": ["warn", {
				allowDestructuring: true,
				allowedNames: ["cursor"],
			}],
			"@typescript-eslint/no-unsafe-declaration-merging": "off",
			"@typescript-eslint/no-unused-expressions": "warn",
			"@typescript-eslint/no-useless-constructor": ["warn"],
			"no-undef": "off", // This is redundant as TypeScript catches things that are really undefined
		},
	},
	{
		files: ["*.js"],
		languageOptions: {
			globals: globals.node,
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

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// React
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	{
		...pluginReact.configs.flat.all,
		...pluginReact.configs.flat["jsx-runtime"],
		files: ["**/*.tsx"],
		plugins: {
			react: pluginReact,
		},
	},

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Import
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	{
		name: "Plugin:import",
		...pluginImport.flatConfigs.typescript,
		files: ["**/*.{ts,tsx}", "eslint.config.mjs"],
		plugins: {
			import: pluginImport,
		},
		rules: {
			"@typescript-eslint/consistent-type-imports": ["warn", { prefer: "type-imports" }],
			"import/consistent-type-specifier-style": ["warn", "prefer-top-level"],
			"import/newline-after-import": "warn",
			"import/no-cycle": ["warn", { ignoreExternal: true }],
			"import/no-duplicates": "warn",
			"import/no-unresolved": "error",
			"import/order": ["warn", {
				"groups": [
					[
						"builtin",
						"external",
					],
					[
						"internal",
						"parent",
						"sibling",
						"index",
						"object",
					],
					"type",
				],
				"newlines-between": "always",
			}],
			"no-duplicate-imports": "off",
			"sort-imports": "off",
		},
		settings: {
			"import/resolver": {
				typescript: {
					project: ["src/app"],
				},
			},
		},
	},

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Specific scopes
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	{
		files: ["gulpfile.js"],
		rules: {
			"@typescript-eslint/no-var-requires": "off",
			"@typescript-eslint/no-require-imports": "off",
		},
	},
];

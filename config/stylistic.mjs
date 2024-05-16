import pluginStyle from "@stylistic/eslint-plugin";

export default {
	...pluginStyle.configs["recommended-flat"],
	rules: {
		"@stylistic/array-bracket-newline": [
			"warn",
			"consistent",
		],
		"@stylistic/array-bracket-spacing": "warn",
		"@stylistic/arrow-spacing": "warn",
		"@stylistic/block-spacing": "warn",
		"@stylistic/brace-style": [
			"warn",
			"1tbs",
			{
				"allowSingleLine": true,
			},
		],
		"@stylistic/comma-dangle": [
			"warn",
			{
				"arrays": "always-multiline",
				"enums": "always-multiline",
				"exports": "never",
				"functions": "never",
				"imports": "never",
				"objects": "always-multiline",
			},
		],
		"@stylistic/comma-spacing": "warn",
		"@stylistic/comma-style": "warn",
		"@stylistic/computed-property-spacing": "warn",
		"@stylistic/dot-location": [
			"warn",
			"property",
		],
		"@stylistic/eol-last": "warn",
		"@stylistic/func-call-spacing": [
			"warn",
			"never",
		],
		"@stylistic/generator-star-spacing": [
			"warn",
			{
				"after": true,
				"anonymous": "neither",
				"before": false,
				"method": {
					"after": false,
					"before": true,
				},
			},
		],
		"@stylistic/jsx-quotes": "warn",
		"@stylistic/key-spacing": [
			"warn",
			{
				"afterColon": true,
				"mode": "strict",
			},
		],
		"@stylistic/linebreak-style": "warn",
		"@stylistic/max-len": [
			"warn",
			{
				"code": 120,
				"ignoreComments": true,
				"ignoreRegExpLiterals": true,
				"ignoreStrings": true,
				"ignoreTemplateLiterals": true,
				"tabWidth": 4,
			},
		],
		"@stylistic/max-statements-per-line": [
			"warn",
			{
				"max": 2,
			},
		],
		"@stylistic/member-delimiter-style": [
			"warn",
			{
				"singleline": {
					"delimiter": "comma",
					"requireLast": false,
				},
			},
		],
		"@stylistic/new-parens": "warn",
		"@stylistic/no-extra-parens": "warn",
		"@stylistic/no-floating-decimal": "warn",
		"@stylistic/no-mixed-spaces-and-tabs": [
			"warn",
			"smart-tabs",
		],
		"@stylistic/no-multi-spaces": "warn",
		"@stylistic/no-multiple-empty-lines": "warn",
		"@stylistic/no-trailing-spaces": "warn",
		"@stylistic/no-whitespace-before-property": "warn",
		"@stylistic/nonblock-statement-body-position": "warn",
		"@stylistic/object-curly-newline": "warn",
		"@stylistic/object-curly-spacing": [
			"warn",
			"always",
		],
		"@stylistic/operator-linebreak": [
			"warn",
			"after",
		],
		"@stylistic/padded-blocks": "off",
		"@stylistic/padding-line-between-statements": "warn",
		"@stylistic/quotes": [
			"warn",
			"double",
			{
				"allowTemplateLiterals": true,
				"avoidEscape": true,
			},
		],
		"@stylistic/rest-spread-spacing": "warn",
		"@stylistic/semi": [
			"warn",
		],
		"@stylistic/semi-spacing": "warn",
		"@stylistic/semi-style": "warn",
		"@stylistic/space-before-blocks": "warn",
		"@stylistic/space-before-function-paren": [
			"warn",
			{
				"anonymous": "never",
				"asyncArrow": "always",
				"named": "never",
			},
		],
		"@stylistic/space-in-parens": "warn",
		"@stylistic/space-infix-ops": [
			"warn",
			{
				"int32Hint": false,
			},
		],
		"@stylistic/space-unary-ops": "warn",
		"@stylistic/switch-colon-spacing": "warn",
		"@stylistic/template-curly-spacing": "warn",
		"@stylistic/template-tag-spacing": "warn",
		"@stylistic/type-annotation-spacing": [
			"warn",
			{
				"after": true,
				"before": false,
				"overrides": {
					"arrow": {
						"before": true,
					},
				},
			},
		],
		"@stylistic/wrap-iife": [
			"warn",
			"inside",
		],
		"@stylistic/wrap-regex": "warn",
		"@stylistic/yield-star-spacing": "warn",
	},
};

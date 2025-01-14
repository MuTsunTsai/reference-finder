import { glob } from "glob";
import { defineConfig } from "@rsbuild/core";
import { pluginReact } from "@rsbuild/plugin-react";
import { pluginCheckSyntax } from "@rsbuild/plugin-check-syntax";
import { GenerateSW } from "workbox-webpack-plugin";
import { PurgeCSSPlugin } from "purgecss-webpack-plugin";
import { pluginAssetsRetry } from "@rsbuild/plugin-assets-retry";
import { pluginSass } from "@rsbuild/plugin-sass";
import { pluginHtmlMinifierTerser } from "rsbuild-plugin-html-minifier-terser";
import { createDescendantRegExp, makeTest } from "@mutsuntsai/rsbuild-utils";

import * as pkg from "./package.json";

const isProduction = process.env.NODE_ENV === "production";

export default defineConfig({
	dev: {
		progressBar: true,
		// See https://github.com/web-infra-dev/rspack/issues/8503
		// lazyCompilation: true,
	},
	source: {
		include: [
			// add matcher for packages that needs to be transpiled
			/i18next/,
			/chart\.js/,
		],
		define: {
			__VERSION__: `"${pkg.version}"`,
		},
		entry: {
			index: "./src/app/index.tsx",
		},
		tsconfigPath: "./src/app/tsconfig.json",
	},
	html: {
		template: "./src/public/index.html",
	},
	server: {
		port: 31213,
		base: "/reference-finder",
		publicDir: {
			name: "src/public",
			copyOnBuild: true,
		},
	},
	performance: {
		chunkSplit: {
			strategy: "custom",
			splitChunks: {
				cacheGroups: {
					rabbit: {
						test: /rabbit-ear/,
						name: "rabbit-ear",
						chunks: "initial",
					},
					react: {
						test: /react/,
						name: "react",
						chunks: "initial",
					},
					vendor: {
						test: /node_modules/,
						name: "vendor",
						chunks: "initial",
						priority: -1,
					},
					ui: {
						test: makeTest(/src[\\/]app[\\/]components/, /node_modules/),
						name: "ui",
						chunks: "async",
						priority: -1,
					},
					chart: {
						test: createDescendantRegExp("chart.js", "chartjs-plugin-datalabels", "react-chartjs-2"),
						name: "chart",
						chunks: "async",
					},
				},
			},
		},
		preload: {
			type: "all-chunks",
			include: [/fa-.+\.woff2$/],
		},
	},
	output: {
		cleanDistPath: isProduction,
		copy: [
			// Uncomment these two lines for WASM debugging
			// { from: "ref.wasm.map", context: "./src/lib/", to: "static/wasm/" },
			// { from: "core/**/*", context: "./src/", to: "static" },
		],
		dataUriLimit: 100,
		legalComments: "none",
		polyfill: "off",
		minify: {
			jsOptions: {
				minimizerOptions: {
					format: {
						asciiOnly: false, // do not escape unicode in strings
					},
				},
			},
		},
		distPath: {
			root: "docs",
		},
	},
	plugins: [
		pluginSass({
			sassLoaderOptions: {
				sassOptions: {
					silenceDeprecations: ["mixed-decls", "color-functions", "import", "global-builtin"],
				},
			},
		}),
		pluginReact(),
		pluginCheckSyntax({
			ecmaVersion: 2018,
		}),
		pluginAssetsRetry({
			addQuery: true,
			test: url => !url.includes("gtag"),
		}),
		pluginHtmlMinifierTerser({
			removeComments: true,
		}),
	],
	tools: {
		rspack: (_, { appendPlugins, isDev }) => {
			if(isDev) return;

			// Using postcss-purgecss is an alternative,
			// but that will result in chunks being processed separately.
			appendPlugins(new PurgeCSSPlugin({
				paths: glob.sync(["./src/app/**/*.tsx", "./src/public/index.html"]),
				variables: true,
				safelist: {
					standard: [
						/backdrop/,
						/bs-theme/,
						/tooltip/,
						/modal-static/,
					],
					greedy: [
						/tooltip-arrow/,
						/creasePattern|boundary/,
						/(point|line|arc|label|target)-/,
					],
				},
				blocklist: [],
			}));

			appendPlugins(new GenerateSW({
				clientsClaim: true,
				skipWaiting: true,
			}));
		},
	},
});

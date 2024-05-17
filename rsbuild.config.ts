import { glob } from "glob";
import { defineConfig } from "@rsbuild/core";
import { pluginReact } from "@rsbuild/plugin-react";
import { pluginCheckSyntax } from "@rsbuild/plugin-check-syntax";
import { GenerateSW } from "workbox-webpack-plugin";
import { PurgeCSSPlugin } from "purgecss-webpack-plugin";

import * as pkg from "./package.json";

const isProduction = process.env.NODE_ENV === "production";

// This is a hack to make purgecss-webpack-plugin working with rspack
(Array.prototype as unknown as Set<unknown>).has = Array.prototype.includes;

export default defineConfig({
	dev: {
		progressBar: true,
	},
	source: {
		include: [
			// add matcher for packages that needs to be transpiled
			/i18next-browser-languagedetector/,
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
		dataUriLimit: 100,
		legalComments: "none",
		assetPrefix: "/reference-finder/",
		polyfill: "off",
		minify: {
			htmlOptions: {
				removeComments: true,
			},
		},
		distPath: {
			root: "docs",
		},
	},
	plugins: [
		pluginReact(),
		pluginCheckSyntax({
			ecmaVersion: 2018,
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

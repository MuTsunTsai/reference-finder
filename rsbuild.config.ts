import { defineConfig } from "@rsbuild/core";
import { pluginReact } from "@rsbuild/plugin-react";
import { pluginCheckSyntax } from '@rsbuild/plugin-check-syntax';
import { GenerateSW } from "workbox-webpack-plugin";
import purgeCss from "@fullhuman/postcss-purgecss";

import * as pkg from "./package.json";

const isProduction = process.env.NODE_ENV === "production";

export default defineConfig({
	dev: {
		progressBar: true,
	},
	source: {
		include: [
			// add matcher for packages that needs to be transpiled
			/i18nextBrowserLanguageDetector/,
			/chart\.js/,
		],
		define: {
			__VERSION__: `"${pkg.version}"`,
		},
		entry: {
			index: "./src/app/index.tsx",
		},
		tsconfigPath: "./src/app/tsconfig.json"
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
	tools: !isProduction ? undefined : {
		postcss: {
			postcssOptions: {
				plugins: [
					purgeCss({
						content: ["./src/app/**/*.tsx", "./src/public/index.html"],
						safelist: {
							standard: [
								/backdrop/,
								/bs-theme/,
								/^--bs-btn-disabled/,
								/^--bs-tooltip/,
							],
							greedy: [
								/tooltip/,
								/creasePattern|boundary/,
								/(point|line|arc|label|target)-/,
							],
						},
					}),
				],
			},
		},
		rspack: (_, { appendPlugins }) => {
			appendPlugins(new GenerateSW({
				clientsClaim: true,
				skipWaiting: true,
			}));
		},
	},
});

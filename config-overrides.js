const paths = require("react-scripts/config/paths");
const glob = require("glob");
const path = require("path");
const { override, addWebpackModuleRule, setWebpackOptimizationSplitChunks, addWebpackPlugin } = require("customize-cra");

const { PurgeCSSPlugin } = require("purgecss-webpack-plugin");
const { GenerateSW } = require("workbox-webpack-plugin");

const rel = p => path.resolve(__dirname, p);

// Custom src and build folders
paths.appSrc = rel("src"); // So that everything in src can be bundled
paths.appIndexJs = rel("src/app/index.tsx");
paths.appPublic = rel("src/public");
paths.appHtml = rel("src/public/index.html");
paths.appBuild = rel("docs");

/**
 * @param {import("webpack").Configuration} config
 * @param {string} env
 */
module.exports = function(config, env) {
	config.resolve.extensions.push(".ts", ".tsx");
	config = override(
		addWebpackModuleRule({
			test: /\.wasm$/,
			type: "asset/resource",
			generator: {
				filename: "static/bin/[name].[contenthash:8][ext]",
			},
		}),
		addWebpackPlugin(new PurgeCSSPlugin({
			paths: glob.sync(`${paths.appSrc}/app/**/*`, { nodir: true }),
			safelist: {
				standard: [/backdrop/], // for Bootstrap Modal
				variables: [
					"--bs-primary",
					/^--bs-btn-disabled/,
				],
			},
		}))
	)(config);
	if(env === "production") {
		config = override(
			// This won't work in debug mode
			setWebpackOptimizationSplitChunks({
				cacheGroups: {
					locale: {
						test: /i18n|locale/,
						chunks: "initial",
						name: "locale",
					},
					rabbitEar: {
						test: /rabbit-ear/,
						chunks: "initial",
						name: "rabbit-ear",
					}
				},
			}),
			addWebpackPlugin(new GenerateSW({
				clientsClaim: true,
				skipWaiting: true,
				include: [
					() => {
						debugger;
						return true;
					}
				]
			}))
		)(config);
	}
	debugger;
	return config;
}

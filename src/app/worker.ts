///<reference lib="webworker" />

import ref from "../lib/ref";

type Action = (...args: unknown[]) => void;

const wasmURL = new URL("../lib/ref.wasm", import.meta.url);

let initialized = false;
let cancelResolve = (value: boolean) => { };
let readyResolve: Action;
let valueResolve: Action | null = null;
const ready = new Promise(resolve => readyResolve = resolve);

const queue: number[] = [];

function put(data: number) {
	if(valueResolve) {
		valueResolve(data);
		valueResolve = null;
	} else {
		queue.push(data);
	}
}

addEventListener('message', async e => {
	if(!e.data) return;
	if(initialized) await ready;
	else initialized = true;
	if(e.data == "cancel") {
		cancelResolve(true);
	} else {
		for(const item of e.data) put(item);
	}
});

ref({
	/**
	 * We use this to hook up with rsbuild (formerly webpack)
	 * See https://gist.github.com/surma/b2705b6cca29357ebea1c9e6e15684cc
	 */
	locateFile(path: string) {
		if(path.endsWith(".wasm")) return wasmURL.pathname;
		return path;
	},
	print: (text: string) => {
		if(text == "Ready") readyResolve();
		postMessage({ text });
	},
	printErr: (err: string) => postMessage({ err }),

	/////////////////////////////////////////////////////////////////////////

	checkCancel: () => new Promise(function(resolve) {
		cancelResolve = resolve;

		// if any message comes in when the tread is blocked,
		// it will be executed before setTimeout
		setTimeout(() => resolve(false), 0);
	}),
	async get() {
		if(queue.length > 0) {
			return queue.shift();
		}
		return await new Promise(resolve => {
			return valueResolve = resolve;
		});
	},
	clear() {
		queue.length = 0;
	},
});

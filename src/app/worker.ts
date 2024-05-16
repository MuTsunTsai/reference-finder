///<reference lib="webworker" />
///<reference types="emscripten" />

import ref from "../lib/ref";

type Action = (...args: unknown[]) => void;

let initialized = false;
let cancelResolve = (_: boolean) => { };
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

addEventListener("message", async e => {
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
} as Partial<EmscriptenModule>);

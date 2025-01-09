import { useDB, useStore, useSettings } from "./store";

import type { DbSettings, Solution } from "./store";

export const idbSupported = typeof indexedDB !== "undefined";

let worker: Worker;
let statisticsCallback: (text: string | Error) => void;

export function useWorker() {
	return worker;
}

const UINT_MAX = 4294967295; // Max value of unsigned int

export function startStatistics(trials: number, callback: typeof statisticsCallback) {
	statisticsCallback = callback;
	worker.postMessage([99, trials, Math.floor(Math.random() * UINT_MAX)]);
}

function parseSolution(text: string) {
	const solution = JSON.parse(text) as Solution;
	const steps = solution.steps;
	solution.steps = [];
	for(const step of steps) {
		if(step.axiom > 0 || step == steps[steps.length - 1]) solution.steps.push(step);
		else solution.steps[solution.steps.length - 1].intersection = step;
	}
	return solution;
}

export function resetWorker(db: DbSettings) {
	let forceRebuild = false;
	if(worker) {
		worker.terminate();
		forceRebuild = true;
		useStore.setState({ running: false, ready: false, progress: null, coreError: null });
		console.log("Reset worker");
	}
	const startTime = performance.now();
	worker = new Worker(
		/* webpackChunkName: "ref" */ new URL("./worker.ts", import.meta.url)
	);
	const { useDB } = useSettings.getState();

	// Get existing marks and lines from the store
	const store = useStore.getState();
	const existingMarks = store.existingMarks;
	const existingLines = store.existingLines;

	// Flatten existingMarks and existingLines into lists of numbers for sending to the core
	const existingMarksFlat = existingMarks.reduce((acc, e: IPoint) => {
		acc.push(e[0], e[1]);
		return acc;
	}, [] as number[]);
	const existingLinesFlat = existingLines.reduce((acc, e: ISegment) => {
		acc.push(e[0][0], e[0][1], e[1][0], e[1][1]);
		return acc;
	}, [] as number[]);

	// Send the message to the worker
	worker.postMessage([
		idbSupported && useDB,
		forceRebuild,
		db.width,
		db.height,
		db.maxRank,
		db.maxLinesV1,
		db.maxMarksV1,
		...db.axiomPriority.map(a => db.axioms[a - 1] ? a : 0),
		db.useDivision,
		db.numX,
		db.numY,
		db.numA,
		db.numD,
		db.minAspectRatio,
		db.minAngleSine,
		db.visibility,
		// Pass in existing lines and marks
		existingMarks.length,
		...existingMarksFlat,
		existingLines.length,
		...existingLinesFlat,
	]);
	worker.onmessage = e => {
		const msg = e.data;
		const { running, ready, solutions, statisticsRunning } = useStore.getState();
		if(msg.text) {
			const text = msg.text;
			if(!ready) {
				if(text.startsWith("{")) {
					useStore.setState({ progress: JSON.parse(text) });
				} else if(text == "Ready") {
					console.log(`Ready in ${Math.floor(performance.now() - startTime)}ms.`);
				} else {
					console.log(text);
				}
			}
			if(text == "Ready") {
				useStore.setState({ running: running && !ready, ready: true });
				return;
			}
			if(!ready) return;

			// uncomment the next line to debug
			// console.log(text);

			if(running) {
				solutions.push(parseSolution(text));
				useStore.setState({ solutions: solutions.concat() });
			} else if(statisticsRunning) {
				statisticsCallback(text);
			}
		}
		if(msg.err) {
			if(!useStore.getState().coreError) {
				// Only set the very first error message received
				useStore.setState({ coreError: msg.err });
			}
			const err = new Error(msg.err);
			console.error(err);
			if(statisticsRunning) statisticsCallback(err);
		}
	};
}

resetWorker(useDB.getState());

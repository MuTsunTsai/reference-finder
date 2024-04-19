import { useDB, useStore } from "./store";

import type { DbSettings, Solution } from "./store";

let worker: Worker;

export function useWorker() {
	return worker;
}

export function resetWorker(db: DbSettings) {
	if(worker) {
		worker.terminate();
		useStore.setState({ running: false, ready: false, progress: null });
		console.log("Reset worker");
	}
	worker = new Worker(
		/* webpackChunkName: "ref" */ new URL("../lib/worker.js", import.meta.url)
	);
	worker.postMessage([
		db.width,
		db.height,
		db.maxRank,
		db.maxLinesV1,
		db.maxMarksV1,
		...db.axioms.map(Number),
		db.numX,
		db.numY,
		db.numA,
		db.numD,
		db.minAspectRatio,
		db.minAngleSine,
	]);
	worker.onmessage = e => {
		const msg = e.data;
		if(msg.text) {
			const text = msg.text;
			const { running, ready, solutions } = useStore.getState();
			if(!ready) {
				if(text.startsWith("{")) useStore.setState({ progress: JSON.parse(text) });
				else console.log(text);
			}
			if(text == "Ready") {
				useStore.setState({ running: running && !ready, ready: true });
				return;
			}
			if(!running || !ready) return;

			// Organize steps
			const solution = JSON.parse(text) as Solution;
			const steps = solution.steps;
			solution.steps = [];
			for(const step of steps) {
				if(step.axiom > 0 || step == steps[steps.length - 1]) solution.steps.push(step);
				else solution.steps[solution.steps.length - 1].intersection = step;
			}
			solutions.push(solution);

			useStore.setState({ solutions: solutions.concat() });
		}
		if(msg.err) {
			useStore.setState({ coreError: msg.err });
			console.error(new Error(msg.err));
		}
	};
}

resetWorker(useDB.getState());

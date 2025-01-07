import ear from "rabbit-ear";

import { Preview } from "./svg/preview";
import { FormEvent, useMemo, useState } from "react";
import { PointInput } from "./form/point-input";
import { useDB, useSettings, useStore } from "../store";
import { useWorker } from "../bridge";
import { Settings } from "./settings/settings";
import { Statistics } from "./statistics/statistics";
import { useTranslation } from "react-i18next";

enum Mode {
	point = 1,
	line = 2,
}

interface PanelProps {
	onSubmit: () => void;
}

export function Panel({ onSubmit }: PanelProps) {
	const { t } = useTranslation();
	const store = useStore();
	const settings = useSettings();
	const db = useDB();
	const [mode, setMode] = useState(Mode.point);
	const [p1, setP1] = useState({ x: 0, y: 0 });
	const [p2, setP2] = useState({ x: 1, y: 1 });

	const cp = useMemo(() => ear.cp.rectangle(db.width, db.height), [db.width, db.height]);
	const points = useMemo(() => {
		const results: IPoint[] = [
			[p1.x, p1.y],
		];
		if(mode == Mode.line) results.push([p2.x, p2.y]);
		return results;
	}, [mode, p1.x, p1.y, p2.x, p2.y]);

	function find(e: FormEvent) {
		e.preventDefault();
		gtag("event", mode == Mode.point ? "ref_find_point" : "ref_find_line");
		const query = [mode, settings.error, settings.count, settings.worstCaseError, p1.x, p1.y];
		if(mode == Mode.line) query.push(p2.x, p2.y);
		useStore.setState({ running: true, solutions: [], coreError: null });
		onSubmit();
		useWorker().postMessage(query.map(Number));
	}

	function editInput(e: FormEvent) {
		e.preventDefault(); // prevent page refresh
		// set store.solutions to empty (this will re-enable the input fields)
		useStore.setState({ solutions: [] });
	}

	function determineSubmitButton() {
		// The last button in this forum is either "Initializing...", "Find Solutions" or "Edit input"
		if (store.solutions.length > 0) { // "Edit input" (in case the user made a mistake)
			return <button className="btn btn-secondary" onClick={editInput}>
				<i className="fas fa-undo"></i>&nbsp;<span className="capitalize">t("phrase.editInput")</span>
			</button>;
		} else if (store.running && !store.ready) { // "Initializing..."
			return <button className="btn btn-primary" disabled>
				<span className="capitalize">{t("phrase.initializing")}&nbsp;<i className="fa-solid fa-spinner fa-spin"></i></span>
			</button>;
		}
		// "Find Solutions"
		return <button className="btn btn-primary" onClick={find}>
			<i className="fa-solid fa-play"></i>&nbsp;<span className="capitalize">t("phrase.findSolutions")</span>
		</button>;
	}

	return (
		<div className="row mt-3 justify-content-center">
			<div className="col mb-3" style={{ flex: "0 1 12rem" }}>
				<Preview cp={cp} points={points} existingCreases={useStore.getState().existingCreaseLines} />
			</div>
			<form className="col mb-3" style={{ flex: "1 0 36rem" }}>
				<div className="row mb-2 pb-1">
					<div className="col-auto">
						<div className="form-check">
							<input
								className="form-check-input"
								type="radio"
								name="mode"
								id="m1"
								checked={mode == Mode.point}
								onChange={() => setMode(Mode.point)}
								disabled={store.solutions.length > 0}
							/>
							<label className="form-check-label capitalize" htmlFor="m1">
								{t("phrase.findPoint")}
							</label>
						</div>
					</div>
					<div className="col-auto">
						<div className="form-check">
							<input
								className="form-check-input"
								type="radio"
								name="mode"
								id="m2"
								checked={mode == Mode.line}
								onChange={() => setMode(Mode.line)}
								disabled={store.solutions.length > 0}
							/>
							<label className="form-check-label capitalize" htmlFor="m2">
								{t("phrase.findLine")}
							</label>
						</div>
					</div>
				</div>
				<PointInput label={mode == Mode.line ? " 1" : ""} value={p1} onInput={p => setP1(p)} />
				{mode == Mode.line &&
					<PointInput label=" 2" value={p2} onInput={p => setP2(p)} />
				}
				<div className="row mt-2 gx-2">
					<div className="col">
						<Settings /> <Statistics />
					</div>
					<div className="col-auto text-end">
						{determineSubmitButton()}
					</div>
				</div>
			</form>
		</div>
	);
}

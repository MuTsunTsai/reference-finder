
import ear from "rabbit-ear";

import { Preview } from "./svg/preview";
import { FormEvent, useMemo, useState } from "react";
import { PointInput } from "./form/point-input";
import { useDB, useStore } from "../store";
import { useWorker } from "../worker";
import { Settings } from "./settings";
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
	const db = useDB();
	const [mode, setMode] = useState(Mode.point);
	const [p1, setP1] = useState({ x: 0, y: 0 });
	const [p2, setP2] = useState({ x: 1, y: 1 });

	const cp = useMemo(() => ear.cp.rectangle(db.width, db.height), [db.width, db.height]);
	const points = useMemo(() => {
		const results: IPoint[] = [
			[p1.x, p1.y]
		];
		if(mode == Mode.line) results.push([p2.x, p2.y]);
		return results;
	}, [mode, p1.x, p1.y, p2.x, p2.y]);

	function find(e: FormEvent) {
		e.preventDefault();
		const query = [mode, p1.x, p1.y];
		if(mode == Mode.line) query.push(p2.x, p2.y);
		useStore.setState({ running: true, solutions: [], coreError: null });
		onSubmit();
		useWorker().postMessage(query.map(Number));
	}

	return (
		<div className="row mt-3 justify-content-center">
			<div className="col mb-3" style={{ flex: "0 1 12rem" }}>
				<Preview cp={cp} points={points} />
			</div>
			<form className="col mb-3" onSubmit={find} style={{ flex: "1 0 36rem" }}>
				<div className="row">
					<div className="col-auto">
						<div className="form-check">
							<input className="form-check-input" type="radio" name="mode" id="m1"
								checked={mode == Mode.point} onChange={() => setMode(Mode.point)} />
							<label className="form-check-label capitalize" htmlFor="m1">
								{t("phrase.findPoint")}
							</label>
						</div>
					</div>
					<div className="col-auto">
						<div className="form-check">
							<input className="form-check-input" type="radio" name="mode" id="m2"
								checked={mode == Mode.line} onChange={() => setMode(Mode.line)} />
							<label className="form-check-label capitalize" htmlFor="m2">
								{t("phrase.findLine")}
							</label>
						</div>
					</div>
				</div>
				<PointInput label={mode == Mode.line ? " 1" : ""} value={p1} onInput={p => setP1(p)} />
				{mode == Mode.line && (
					<PointInput label=" 2" value={p2} onInput={p => setP2(p)} />
				)}
				<div className="row mt-2">
					<div className="col">
						<Settings />
					</div>
					<div className="col-auto text-end">
						<button type="submit" className="btn btn-primary capitalize" disabled={store.running}>
							{store.running && !store.ready ? (
								<span>{t("phrase.initializing")}&nbsp;<i className="fa-solid fa-spinner fa-spin"></i></span>
							) : (
								<span><i className="fa-solid fa-play"></i>&nbsp;{t("phrase.go")}</span>
							)}
						</button>
					</div>
				</div>
			</form>
		</div>
	);
}

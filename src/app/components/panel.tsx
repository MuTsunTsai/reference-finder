import { useMemo, useState } from "react";
import { useTranslation } from "react-i18next";

import { Preview } from "./svg/preview";
import { PointInput } from "./form/point-input";
import { CpInput } from "./cp/cpInput";
import { useDB, useSettings, useStore } from "../store";
import { Command, resetWorker, useWorker } from "../bridge";
import { Settings } from "./settings/settings";
import { Statistics } from "./statistics/statistics";
import { getTargetError } from "../restriction";
import { fitToSheet, getSelectionPoints } from "../cp/parse";

import type { CpSelection, CreasePattern } from "../cp/parse";
import type { FormEvent } from "react";

interface PanelProps {
	onSubmit: () => void;
}

enum Mode {
	point,
	line,
	/** Selecting a point or a line from a crease pattern. */
	cp,
}

export function Panel({ onSubmit }: PanelProps) {
	const { t } = useTranslation();
	const store = useStore();
	const settings = useSettings();
	const db = useDB();
	const [selectedMode, setMode] = useState(Mode.point);
	// When marks are restricted, finding lines doesn't make sense,
	// as lines would require creases in the interior.
	const lineDisabled = db.restrictMarks;
	const mode = lineDisabled && selectedMode == Mode.line ?
		Mode.point :
		selectedMode;
	const [p1, setP1] = useState({ x: 0, y: 0 });
	const [p2, setP2] = useState({ x: 1, y: 1 });
	const [cp, setCp] = useState<CreasePattern | null>(null);
	const [cpSelection, setCpSelection] = useState<CpSelection | null>(null);

	const fitted = useMemo(
		() => cp && fitToSheet(cp, db.width, db.height),
		[cp, db.width, db.height]
	);

	/**
	 * The target point, or the two points defining the target line;
	 * null if nothing is selected.
	 */
	const target = useMemo((): IPoint[] | null => {
		if(mode == Mode.cp) {
			return fitted && cpSelection ?
				getSelectionPoints(fitted, cpSelection) :
				null;
		}
		if(mode == Mode.line) return [[p1.x, p1.y], [p2.x, p2.y]];
		return [[p1.x, p1.y]];
	}, [mode, p1.x, p1.y, p2.x, p2.y, fitted, cpSelection]);

	const targetError = useMemo(() => {
		const key = target &&
			getTargetError(target, db, store.existingLines, settings.error);
		return key ? t(key) : null;
	}, [target, db, store.existingLines, settings.error, t]);

	function find(e: FormEvent) {
		e.preventDefault();
		if(!target || targetError) return;
		const command = target.length == 2 ? Command.line : Command.mark;
		gtag("event", command == Command.mark ? "ref_find_point" : "ref_find_line");
		const query = [
			command,
			settings.error,
			settings.count,
			settings.worstCaseError,
			...target.flat(),
		];
		useStore.setState({ running: true, solutions: [], coreError: null });
		onSubmit();
		useWorker().postMessage(query.map(Number));
	}

	function clearExistingRefs() {
		useStore.setState({
			existingRefs: [],
			existingLines: [],
			existingMarks: [],
			solutions: [],
		});
		resetWorker(useDB.getState(), false);
	}

	function loadCp(result: CreasePattern) {
		setCp(result);
		setCpSelection(null); // The indices no longer apply
	}

	function radio(m: Mode, label: string, disabled?: boolean) {
		const id = "mode" + m;
		return (
			<div className="col-auto">
				<div className="form-check">
					<input
						className="form-check-input"
						type="radio"
						name="mode"
						id={id}
						checked={mode == m}
						disabled={disabled}
						onChange={() => setMode(m)}
					/>
					<label className="form-check-label capitalize" htmlFor={id}>
						{label}
					</label>
				</div>
			</div>
		);
	}

	const hasExistingRefs =
		store.existingMarks.length > 0 || store.existingLines.length > 0;

	return (
		<div className="row mt-3 justify-content-center">
			<div className="col mb-3" style={{ flex: "0 1 12rem" }}>
				<Preview points={target ?? []} />
			</div>
			<form className="col mb-3" style={{ flex: "1 0 36rem" }} onSubmit={find}>
				<div className="row mb-2 pb-1">
					{radio(Mode.point, t("phrase.findPoint"))}
					{radio(Mode.line, t("phrase.findLine"), lineDisabled)}
					{radio(Mode.cp, t("phrase.fromCP"))}
				</div>
				{mode == Mode.cp ?
					<CpInput
						cp={cp}
						fitted={fitted}
						selection={cpSelection}
						error={targetError}
						onLoad={loadCp}
						onSelect={setCpSelection}
					/> :
					<>
						<PointInput
							label={mode == Mode.line ? " 1" : ""}
							value={p1}
							error={mode == Mode.point ? targetError : null}
							onInput={p => setP1(p)}
						/>
						{mode == Mode.line &&
							<PointInput label=" 2" value={p2} onInput={p => setP2(p)} />
						}
					</>
				}
				<div className="row mt-2 gx-2">
					<div className="col">
						<Settings /> <Statistics />	{
							hasExistingRefs &&
							<button type="button" className="btn btn-secondary me-1" onClick={clearExistingRefs}>
								<i className="fa-solid fa-trash"></i><span className="d-none d-sm-inline">&nbsp;<span className="capitalize">{t("phrase.clearExistingRefs")}</span></span>
							</button>
						}
					</div>
					<div className="col-auto text-end">
						{
							(store.running && !store.ready) ?
								<button type="submit" className="btn btn-primary" disabled>
									<span className="capitalize">{t("phrase.initializing")}&nbsp;<i className="fa-solid fa-spinner fa-spin"></i></span>
								</button> :
								<button type="submit" className="btn btn-primary" disabled={!target}>
									<i className="fa-solid fa-play"></i>&nbsp;<span className="capitalize">{t("phrase.go")}</span>
								</button>
						}
					</div>
				</div>
			</form>
		</div>
	);
}

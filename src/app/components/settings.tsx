import { useRef, useState } from "react";
import { Modal } from "bootstrap";
import { ExpInput } from "./exp-input";
import { IntInput } from "./int-input";
import { Theme, defaultDB, defaultSettings, useDB, useSettings } from "../store";
import { resetWorker } from "../worker";
import { SettingsRow } from "./settings-row";
import { Checkbox } from "./checkbox";
import { useTranslation } from "react-i18next";

export function Settings() {
	const { t } = useTranslation();
	const db = useDB();
	const settings = useSettings();
	const [tempSettings, setTempSettings] = useState(structuredClone(settings));
	const [tempDb, setTempDb] = useState(structuredClone(db));
	const [tab, setTab] = useState(0);
	const ref = useRef(null);
	const handleShow = () => {
		setTempSettings(structuredClone(settings));
		setTempDb(structuredClone(db));
		Modal.getOrCreateInstance(ref.current!, { backdrop: "static" }).show();
	};
	const handleSave = () => {
		useSettings.setState(tempSettings);
		if(hasChanged(db, tempDb)) {
			useDB.setState(tempDb);
			resetWorker(tempDb);
		}
	};
	const setAxiom = (index: number, v: boolean) => {
		const axioms = tempDb.axioms.concat();
		axioms[index] = v;
		setTempDb({ ...tempDb, axioms });
	}

	const reset = () => {
		setTempSettings(structuredClone(defaultSettings));
		setTempDb(structuredClone(defaultDB));
	}

	return (
		<>
			<button type="button" className="btn btn-secondary capitalize" onClick={handleShow}>
				<i className="fa-solid fa-gear"></i>&nbsp;{t("phrase.settings")}
			</button>
			<div className="modal fade" ref={ref}>
				<div className="modal-dialog modal-dialog-centered">
					<div className="modal-content">
						<div className="modal-body">
							<ul className="nav nav-tabs mb-3">
								<li className="nav-item">
									<a className={"nav-link " + (tab == 0 ? "active" : "")} onClick={() => setTab(0)}>Basic</a>
								</li>
								<li className="nav-item">
									<a className={"nav-link " + (tab == 1 ? "active" : "")} onClick={() => setTab(1)}>Advanced</a>
								</li>
								<li className="nav-item">
									<a className={"nav-link " + (tab == 2 ? "active" : "")} onClick={() => setTab(2)}>Display</a>
								</li>
							</ul>
							<div className={(tab == 0 ? "" : "d-none")}>
								<SettingsRow label="Sheet width:">
									<ExpInput value={tempDb.width} exp={tempDb.widthExp}
										onInput={(v, exp) => setTempDb({ ...tempDb, width: v, widthExp: exp })} />
								</SettingsRow>
								<SettingsRow label="Sheet height:">
									<ExpInput value={tempDb.height} exp={tempDb.heightExp}
										onInput={(v, exp) => setTempDb({ ...tempDb, height: v, heightExp: exp })} />
								</SettingsRow>
								<SettingsRow label="Max rank:">
									<IntInput value={tempDb.maxRank} min={2} onInput={v => setTempDb({ ...tempDb, maxRank: v })} />
								</SettingsRow>
								<SettingsRow label="Max lines:">
									<IntInput value={tempDb.maxLines} onInput={v => setTempDb({ ...tempDb, maxLines: v })} />
								</SettingsRow>
								<SettingsRow label="Max marks:">
									<IntInput value={tempDb.maxMarks} onInput={v => setTempDb({ ...tempDb, maxMarks: v })} />
								</SettingsRow>
								<hr />
								<div className="mb-1">
									<h6>Huzita-Hatori axioms:</h6>
									<Checkbox value={tempDb.axioms[0]} onInput={v => setAxiom(0, v)}>O1 - Crease through two points.</Checkbox>
									<Checkbox value={tempDb.axioms[1]} onInput={v => setAxiom(1, v)}>O2 - Two points together.</Checkbox>
									<Checkbox value={tempDb.axioms[2]} onInput={v => setAxiom(2, v)}>O3 - Line to another line.</Checkbox>
									<Checkbox value={tempDb.axioms[3]} onInput={v => setAxiom(3, v)}>O4 - Line to self, crease through point.</Checkbox>
									<Checkbox value={tempDb.axioms[4]} onInput={v => setAxiom(4, v)}>O5 - Point to line, crease through point.</Checkbox>
									<Checkbox value={tempDb.axioms[5]} onInput={v => setAxiom(5, v)}>O6 - Two points to two lines.</Checkbox>
									<Checkbox value={tempDb.axioms[6]} onInput={v => setAxiom(6, v)}>O7 - Line to self, point to line.</Checkbox>
								</div>
							</div>
							<div className={(tab == 1 ? "" : "d-none")}>
								<SettingsRow label="X divisions:">
									<IntInput value={tempDb.numX} min={100} onInput={v => setTempDb({ ...tempDb, numX: v })} />
								</SettingsRow>
								<SettingsRow label="Y divisions:">
									<IntInput value={tempDb.numY} min={100} onInput={v => setTempDb({ ...tempDb, numY: v })} />
								</SettingsRow>
								<SettingsRow label="Angle divisions:">
									<IntInput value={tempDb.numA} min={100} onInput={v => setTempDb({ ...tempDb, numA: v })} />
								</SettingsRow>
								<SettingsRow label="Radial divisions:">
									<IntInput value={tempDb.numD} min={100} onInput={v => setTempDb({ ...tempDb, numD: v })} />
								</SettingsRow>
								<SettingsRow label="Good enough error:">
									<ExpInput value={tempDb.error} exp={tempDb.errorExp}
										onInput={(v, exp) => setTempDb({ ...tempDb, error: v, errorExp: exp })} />
								</SettingsRow>
								<SettingsRow label="Min aspect ratio:">
									<ExpInput value={tempDb.minAspectRatio} exp={tempDb.minAspectRatioExp}
										onInput={(v, exp) => setTempDb({ ...tempDb, minAspectRatio: v, minAspectRatioExp: exp })} />
								</SettingsRow>
								<SettingsRow label="Min angle sine:">
									<ExpInput value={tempDb.minAngleSine} exp={tempDb.minAngleSineExp}
										onInput={(v, exp) => setTempDb({ ...tempDb, minAngleSine: v, minAngleSineExp: exp })} />
								</SettingsRow>
							</div>
							<div className={(tab == 2 ? "" : "d-none")}>
								<SettingsRow label="Theme:">
									<select value={tempSettings.theme} className="form-select"
										onChange={e => setTempSettings({ ...tempSettings, theme: Number(e.currentTarget.value) })} >
										<option value={Theme.system}>System</option>
										<option value={Theme.light}>Light</option>
										<option value={Theme.dark}>Dark</option>
										<option value={Theme.rabbitEar}>RabbitEar</option>
									</select>
								</SettingsRow>
								<div className="mt-3 mb-1">
									<Checkbox value={tempSettings.showAxiom}
										onInput={v => setTempSettings({ ...tempSettings, showAxiom: v })}>Show axiom number in description.</Checkbox>
								</div>
							</div>
							{hasChanged(db, tempDb) && (
								<div className="text-warning mt-2">* Requires re-initializing.</div>
							)}
						</div>
						<div className="modal-footer">
							<div className="flex-grow-1">
								<button className="btn btn-secondary" onClick={reset}>{t("settings.reset")}</button>
							</div>
							<div>
								<button className="btn btn-secondary me-2 capitalize" data-bs-dismiss="modal">{t("settings.cancel")}</button>
								<button className="btn btn-primary" data-bs-dismiss="modal" onClick={handleSave}>{t("settings.ok")}</button>
							</div>
						</div>
					</div>
				</div>
			</div>
		</>
	);
}

function hasChanged<T extends object>(oldV: T, newV: T): boolean {
	for(const key in oldV) {
		if(Array.isArray(oldV[key])) {
			if(hasChanged(oldV[key] as unknown[], newV[key] as unknown[])) return true;
		} else {
			if(oldV[key] !== newV[key]) return true;
		}
	}
	return false;
}

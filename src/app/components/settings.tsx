import { useRef, useState } from "react";
import { Modal } from "bootstrap";
import { useTranslation } from "react-i18next";

import { ExpInput } from "./form/exp-input";
import { IntInput } from "./form/int-input";
import { Theme, defaultDB, defaultSettings, useDB, useSettings } from "../store";
import { resetWorker } from "../worker";
import { SettingsRow } from "./form/settings-row";
import { Checkbox } from "./form/checkbox";

import "./settings.scss";
import { InfoTooltip } from "./tooltip";

export function Settings() {
	const { t } = useTranslation();
	const db = useDB();
	const settings = useSettings();
	const [cacheSettings, cacheTempSettings] = useState(structuredClone(settings));
	const [tempDb, setTempDb] = useState(structuredClone(db));
	const [tab, setTab] = useState(0);
	const ref = useRef(null);
	const handleShow = () => {
		cacheTempSettings(structuredClone(settings));
		setTempDb(structuredClone(db));
		Modal.getOrCreateInstance(ref.current!, { backdrop: "static" }).show();
	};
	const handleSave = () => {
		if(hasChanged(db, tempDb)) {
			useDB.setState(tempDb);
			resetWorker(tempDb);
		}
	};
	const handleCancel = () => {
		useSettings.setState(cacheSettings);
	}
	const setAxiom = (index: number, v: boolean) => {
		const axioms = tempDb.axioms.concat();
		axioms[index] = v;
		setTempDb({ ...tempDb, axioms });
	}

	const reset = () => {
		// Settings are written in-place, so that it can be reflected on UI immediately
		useSettings.setState(structuredClone(defaultSettings));
		setTempDb(structuredClone(defaultDB));
	}

	return (
		<>
			<button type="button" className="btn btn-secondary" onClick={handleShow}>
				<i className="fa-solid fa-gear"></i>&nbsp;<span className="capitalize">{t("phrase.settings")}</span>
			</button>
			<div className="modal fade" ref={ref}>
				<div className="modal-dialog modal-dialog-centered">
					<div className="modal-content">
						<div className="modal-body">
							<ul className="nav nav-tabs mb-3">
								<li className="nav-item">
									<a className={"nav-link " + (tab == 0 ? "active" : "")} onClick={() => setTab(0)}>{t("settings.basic._")}</a>
								</li>
								<li className="nav-item">
									<a className={"nav-link " + (tab == 1 ? "active" : "")} onClick={() => setTab(1)}>{t("settings.advanced._")}</a>
								</li>
								<li className="nav-item">
									<a className={"nav-link " + (tab == 2 ? "active" : "")} onClick={() => setTab(2)}>{t("settings.display._")}</a>
								</li>
							</ul>
							<div className={(tab == 0 ? "" : "d-none")}>
								<div className="grid">
									<SettingsRow label={t("settings.basic.width")}>
										<ExpInput value={tempDb.width} exp={tempDb.widthExp}
											onInput={(v, exp) => setTempDb({ ...tempDb, width: v, widthExp: exp })} />
									</SettingsRow>
									<SettingsRow label={t("settings.basic.height")}>
										<ExpInput value={tempDb.height} exp={tempDb.heightExp}
											onInput={(v, exp) => setTempDb({ ...tempDb, height: v, heightExp: exp })} />
									</SettingsRow>
									<SettingsRow label="Max rank" help={t("help.maxRank")}>
										<IntInput value={tempDb.maxRank} min={2} onInput={v => setTempDb({ ...tempDb, maxRank: v })} />
									</SettingsRow>
									<SettingsRow label="Max lines" help={t("help.maxLines")}>
										<IntInput value={tempDb.maxLinesV1} onInput={v => setTempDb({ ...tempDb, maxLinesV1: v })} />
									</SettingsRow>
									<SettingsRow label="Max marks" help={t("help.maxMarks")}>
										<IntInput value={tempDb.maxMarksV1} onInput={v => setTempDb({ ...tempDb, maxMarksV1: v })} />
									</SettingsRow>
								</div>
								<hr />
								<div className="mb-1">
									<h6>{t("settings.basic.axioms._")}<InfoTooltip title={t("help.axioms")} /></h6>
									<Checkbox value={tempDb.axioms[0]} onInput={v => setAxiom(0, v)}>O1 - {t("settings.basic.axioms.O1")}</Checkbox>
									<Checkbox value={tempDb.axioms[1]} onInput={v => setAxiom(1, v)}>O2 - {t("settings.basic.axioms.O2")}</Checkbox>
									<Checkbox value={tempDb.axioms[2]} onInput={v => setAxiom(2, v)}>O3 - {t("settings.basic.axioms.O3")}</Checkbox>
									<Checkbox value={tempDb.axioms[3]} onInput={v => setAxiom(3, v)}>O4 - {t("settings.basic.axioms.O4")}</Checkbox>
									<Checkbox value={tempDb.axioms[4]} onInput={v => setAxiom(4, v)}>O5 - {t("settings.basic.axioms.O5")}</Checkbox>
									<Checkbox value={tempDb.axioms[5]} onInput={v => setAxiom(5, v)}>O6 - {t("settings.basic.axioms.O6")}</Checkbox>
									<Checkbox value={tempDb.axioms[6]} onInput={v => setAxiom(6, v)}>O7 - {t("settings.basic.axioms.O7")}</Checkbox>
								</div>
							</div>
							<div className={"grid " + (tab == 1 ? "" : "d-none")}>
								<SettingsRow label="X divisions" help={t("help.divisions")}>
									<IntInput value={tempDb.numX} min={100} max={46000} onInput={v => setTempDb({ ...tempDb, numX: v })} />
								</SettingsRow>
								<SettingsRow label="Y divisions" help={t("help.divisions")}>
									<IntInput value={tempDb.numY} min={100} max={46000} onInput={v => setTempDb({ ...tempDb, numY: v })} />
								</SettingsRow>
								<SettingsRow label="Angle divisions" help={t("help.lineDivisions")}>
									<IntInput value={tempDb.numA} min={100} max={46000} onInput={v => setTempDb({ ...tempDb, numA: v })} />
								</SettingsRow>
								<SettingsRow label="Radial divisions" help={t("help.lineDivisions")}>
									<IntInput value={tempDb.numD} min={100} max={46000} onInput={v => setTempDb({ ...tempDb, numD: v })} />
								</SettingsRow>
								<SettingsRow label="Min aspect ratio" help={t("help.minAspectRatio")}>
									<ExpInput value={tempDb.minAspectRatio} exp={tempDb.minAspectRatioExp}
										onInput={(v, exp) => setTempDb({ ...tempDb, minAspectRatio: v, minAspectRatioExp: exp })} />
								</SettingsRow>
								<SettingsRow label="Min angle sine" help={t("help.minAngle")}>
									<ExpInput value={tempDb.minAngleSine} exp={tempDb.minAngleSineExp}
										onInput={(v, exp) => setTempDb({ ...tempDb, minAngleSine: v, minAngleSineExp: exp })} />
								</SettingsRow>
								<SettingsRow label="Visibility matters" help={t("help.visibility")}>
									<Checkbox className="col-form-label" value={tempDb.visibility} onInput={v => setTempDb({ ...tempDb, visibility: v })} />
								</SettingsRow>
								<div className="mt-n2" style={{ gridColumn: "1/3" }}><hr /></div>
								<SettingsRow label="Line error mode" help={t("help.lineError")}>
										<select className="form-select" value={settings.worstCaseError ? "1" : ""}
											onChange={e => useSettings.setState({ worstCaseError: Boolean(e.currentTarget.value) })}>
											<option value="1">Worst-case error</option>
											<option value="">Pythagorean error</option>
										</select>
									</SettingsRow>
								<SettingsRow label="Good enough error" help={t("help.goodEnoughError")}>
									<ExpInput value={settings.error} exp={settings.errorExp}
										onInput={(v, exp) => useSettings.setState({ error: v, errorExp: exp })} />
								</SettingsRow>
								<SettingsRow label="Solution count">
									<IntInput min={1} max={20} value={settings.count}
										onInput={v => useSettings.setState({ count: v })} />
								</SettingsRow>
							</div>
							<div className={(tab == 2 ? "" : "d-none")}>
								<div className="grid">
									<SettingsRow label={t("settings.display.theme._")}>
										<select value={settings.theme} className="form-select"
											onChange={e => useSettings.setState({ theme: Number(e.currentTarget.value) })} >
											<option value={Theme.system}>{t("settings.display.theme.system")}</option>
											<option value={Theme.light}>{t("settings.display.theme.light")}</option>
											<option value={Theme.dark}>{t("settings.display.theme.dark")}</option>
											<option value={Theme.rabbitEar}>RabbitEar</option>
										</select>
									</SettingsRow>
								</div>
								<div className="mt-3 mb-1">
									<Checkbox value={settings.showInit}
										onInput={v => useSettings.setState({ showInit: v })}>{t("settings.display.showInit")}</Checkbox>
									<Checkbox value={settings.showAxiom}
										onInput={v => useSettings.setState({ showAxiom: v })}>{t("settings.display.showAxiom")}</Checkbox>
								</div>
							</div>
							{hasChanged(db, tempDb) && (
								<div className="text-warning mt-2">* {t("settings.reInit")}</div>
							)}
						</div>
						<div className="modal-footer">
							<div className="flex-grow-1">
								<button type="button" className="btn btn-secondary" onClick={reset}>{t("settings.reset")}</button>
							</div>
							<div>
								<button type="button" className="btn btn-secondary me-2 capitalize" data-bs-dismiss="modal" onClick={handleCancel}>{t("settings.cancel")}</button>
								<button type="button" className="btn btn-primary" data-bs-dismiss="modal" onClick={handleSave}>{t("settings.ok")}</button>
							</div>
						</div>
					</div>
				</div>
			</div >
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

import { useRef, useState } from "react";
import { Modal } from "bootstrap";
import { useTranslation } from "react-i18next";

import { ExpInput } from "./exp-input";
import { IntInput } from "./int-input";
import { Theme, defaultDB, defaultSettings, useDB, useSettings } from "../store";
import { resetWorker } from "../worker";
import { SettingsRow } from "./settings-row";
import { Checkbox } from "./checkbox";

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
		cacheTempSettings(structuredClone(defaultSettings));
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
									<SettingsRow label="Max rank" help="This sets the maximum rank for any sequence, which is the number of folds involved in the sequence. ReferenceFinder will calculate all marks and lines up to the maximum rank, subject to the two following limits. As a practical matter, you will rarely need to set this any larger than 6 or 7; rank 6 can include over a million distinct lines.">
										<IntInput value={tempDb.maxRank} min={2} onInput={v => setTempDb({ ...tempDb, maxRank: v })} />
									</SettingsRow>
									<SettingsRow label="Max lines" help="This sets an absolute upper bound on the number of lines to calculate. Even if you are only searching for points, ReferenceFinder needs to calculate a large number of lines since points are defined by the intersections of existing lines.">
										<IntInput value={tempDb.maxLinesV1} onInput={v => setTempDb({ ...tempDb, maxLinesV1: v })} />
									</SettingsRow>
									<SettingsRow label="Max marks" help="This sets a absolute upper bound on the number of marks to calculate. As a rough estimate, if this setting is N, the average error in any given point is proportional to 1/(2*sqrt(N)). This is not exact, since points tend to cluster along low-rank lines, but as a good rule of thumb, half a million to a million points will give a good error performance.">
										<IntInput value={tempDb.maxMarksV1} onInput={v => setTempDb({ ...tempDb, maxMarksV1: v })} />
									</SettingsRow>
								</div>

								<hr />
								<div className="mb-1">
									<h6>Huzita-Hatori axioms <InfoTooltip title="The Huzita-Hatori Axioms group lets you choose which of the 7 axioms to include in the database. The default is to include all 7, but you can exclude one or more. Of course, the more axioms you exclude, the higher the rank you'll need to allow to obtain a given level of accuracy."/></h6>
									<Checkbox value={tempDb.axioms[0]} onInput={v => setAxiom(0, v)}>O1 - Crease through two points.</Checkbox>
									<Checkbox value={tempDb.axioms[1]} onInput={v => setAxiom(1, v)}>O2 - Two points together.</Checkbox>
									<Checkbox value={tempDb.axioms[2]} onInput={v => setAxiom(2, v)}>O3 - Line to another line.</Checkbox>
									<Checkbox value={tempDb.axioms[3]} onInput={v => setAxiom(3, v)}>O4 - Line to self, crease through point.</Checkbox>
									<Checkbox value={tempDb.axioms[4]} onInput={v => setAxiom(4, v)}>O5 - Point to line, crease through point.</Checkbox>
									<Checkbox value={tempDb.axioms[5]} onInput={v => setAxiom(5, v)}>O6 - Two points to two lines.</Checkbox>
									<Checkbox value={tempDb.axioms[6]} onInput={v => setAxiom(6, v)}>O7 - Line to self, point to line.</Checkbox>
								</div>
							</div>
							<div className={"grid " + (tab == 1 ? "" : "d-none")}>
								<SettingsRow label="X divisions" help="To avoid duplicates, ReferenceFinder keeps the lowest-rank sequence for any given mark or line. To specifically test for equivalence, the paper is divided up into a fine grid, according to the divisions settings. Two marks are considered equivalent if they fall within the same grid square, and only one of them will be kept.">
									<IntInput value={tempDb.numX} min={100} onInput={v => setTempDb({ ...tempDb, numX: v })} />
								</SettingsRow>
								<SettingsRow label="Y divisions" help="To avoid duplicates, ReferenceFinder keeps the lowest-rank sequence for any given mark or line. To specifically test for equivalence, the paper is divided up into a fine grid, according to the divisions settings. Two marks are considered equivalent if they fall within the same grid square, and only one of them will be kept.">
									<IntInput value={tempDb.numY} min={100} onInput={v => setTempDb({ ...tempDb, numY: v })} />
								</SettingsRow>
								<SettingsRow label="Angle divisions" help="Lines are characterized by their angle and distance of closest approach to the origin, and so these two settings are similarly quantized.">
									<IntInput value={tempDb.numA} min={100} onInput={v => setTempDb({ ...tempDb, numA: v })} />
								</SettingsRow>
								<SettingsRow label="Radial divisions" help="Lines are characterized by their angle and distance of closest approach to the origin, and so these two settings are similarly quantized.">
									<IntInput value={tempDb.numD} min={100} onInput={v => setTempDb({ ...tempDb, numD: v })} />
								</SettingsRow>
								<SettingsRow label="Good enough error" help="When a search is performed, ReferenceFinder finds all marks (or lines) whose distance error (or an analogous quantity for lines) is better than this value. Essentially, this value is the error that you're willing to tolerate. Within this error, low-rank marks are considered more desirable (easier to fold) and so they will be displayed before higher-rank marks, even if the latter have lower errors.">
									<ExpInput value={tempDb.error} exp={tempDb.errorExp}
										onInput={(v, exp) => setTempDb({ ...tempDb, error: v, errorExp: exp })} />
								</SettingsRow>
								<SettingsRow label="Min aspect ratio" help="As a practical matter, it is very difficult to fold a long, skinny corner so that the resulting crease is straight; it is very easy for the crease to wander. If a given sequence requires the folding of a triangular flap with an aspect ratio smaller than this, it is not accepted into the database. If you don't care about this issue, you can set the value to zero.">
									<ExpInput value={tempDb.minAspectRatio} exp={tempDb.minAspectRatioExp}
										onInput={(v, exp) => setTempDb({ ...tempDb, minAspectRatio: v, minAspectRatioExp: exp })} />
								</SettingsRow>
								<SettingsRow label="Min angle sine" help="Similarly, if two creases intersect at a very shallow angle, the location of the point at their intersection is inherently imprecise. This value is the sine of the minimum angle allowed for such intersections. If you don't care about this issue, you can set the value to zero.">
									<ExpInput value={tempDb.minAngleSine} exp={tempDb.minAngleSineExp}
										onInput={(v, exp) => setTempDb({ ...tempDb, minAngleSine: v, minAngleSineExp: exp })} />
								</SettingsRow>
							</div>
							<div className={(tab == 2 ? "" : "d-none")}>
								<div className="grid">
									<SettingsRow label={t("settings.display.theme._")}>
										<select value={settings.theme} className="form-select"
											onChange={e => useSettings.setState({ ...settings, theme: Number(e.currentTarget.value) })} >
											<option value={Theme.system}>{t("settings.display.theme.system")}</option>
											<option value={Theme.light}>{t("settings.display.theme.light")}</option>
											<option value={Theme.dark}>{t("settings.display.theme.dark")}</option>
											<option value={Theme.rabbitEar}>RabbitEar</option>
										</select>
									</SettingsRow>
								</div>
								<div className="mt-3 mb-1">
									<Checkbox value={settings.showAxiom}
										onInput={v => useSettings.setState({ ...settings, showAxiom: v })}>{t("settings.display.showAxiom")}</Checkbox>
								</div>
							</div>
							{hasChanged(db, tempDb) && (
								<div className="text-warning mt-2">* {t("settings.reInit")}</div>
							)}
						</div>
						<div className="modal-footer">
							<div className="flex-grow-1">
								<button className="btn btn-secondary" onClick={reset}>{t("settings.reset")}</button>
							</div>
							<div>
								<button className="btn btn-secondary me-2 capitalize" data-bs-dismiss="modal" onClick={handleCancel}>{t("settings.cancel")}</button>
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

import { useContext, useEffect, useState } from "react";
import { useTranslation } from "react-i18next";
import { deleteDB } from "idb";

import { idbSupported } from "../../bridge";
import { ExpInput } from "../form/exp-input";
import { IntInput } from "../form/int-input";
import { Theme, useSettings } from "../../store";
import { SettingsRow } from "../form/settings-row";
import { Checkbox } from "../form/checkbox";
import { Axioms } from "./axioms";
import { SettingsContext } from "./context";

export default function SettingsBody() {
	const { t } = useTranslation();
	const settings = useSettings();
	const { tempDb, setTempDb } = useContext(SettingsContext);
	const [tab, setTab] = useState(0);

	useEffect(() => {
		if(!settings.useDB) deleteDB("/data");
	}, [settings.useDB]);

	const MAX_DIV = 65_000; // About the sqrt of max unsigned int
	const MIN_OBJ = 10_000;
	const MAX_OBJ = 16_000_000; // This is roughly the max number of references that can be generated under the 4GB limit

	// Disabling divisions works very poorly for ranks >= 6 in general,
	// so we place this restriction to save our users the trouble of finding that fact themselves.
	const mustUseDivision = tempDb.maxRank > 5;
	const disableDivisions = !mustUseDivision && !tempDb.useDivision;

	return (
		<div className="modal-body">
			<ul className="nav nav-tabs mb-3">
				<li className="nav-item">
					<span className={"nav-link " + (tab == 0 ? "active" : "")} onClick={() => setTab(0)}>{t("settings.basic._")}</span>
				</li>
				<li className="nav-item">
					<span className={"nav-link " + (tab == 1 ? "active" : "")} onClick={() => setTab(1)}>{t("settings.advanced._")}</span>
				</li>
				<li className="nav-item">
					<span className={"nav-link " + (tab == 2 ? "active" : "")} onClick={() => setTab(2)}>{t("settings.system._")}</span>
				</li>
			</ul>
			<div className={(tab == 0 ? "" : "d-none")}>
				<div className="grid">
					<SettingsRow label={t("settings.basic.width")}>
						<ExpInput
							value={tempDb.width}
							exp={tempDb.widthExp}
							onInput={(v, exp) => setTempDb({ ...tempDb, width: v, widthExp: exp })}
						/>
					</SettingsRow>
					<SettingsRow label={t("settings.basic.height")}>
						<ExpInput
							value={tempDb.height}
							exp={tempDb.heightExp}
							onInput={(v, exp) => setTempDb({ ...tempDb, height: v, heightExp: exp })}
						/>
					</SettingsRow>
					<SettingsRow label="Max rank" help={t("help.maxRank")}>
						<IntInput
							value={tempDb.maxRank}
							min={2}
							onInput={v => setTempDb({ ...tempDb, maxRank: v })}
						/>
					</SettingsRow>
					<SettingsRow label="Max lines" help={t("help.maxLines")}>
						<IntInput
							value={tempDb.maxLinesV1}
							min={MIN_OBJ}
							max={MAX_OBJ}
							onInput={v => setTempDb({ ...tempDb, maxLinesV1: v })}
						/>
					</SettingsRow>
					<SettingsRow label="Max marks" help={t("help.maxMarks")}>
						<IntInput
							value={tempDb.maxMarksV1}
							min={MIN_OBJ}
							max={MAX_OBJ}
							onInput={v => setTempDb({ ...tempDb, maxMarksV1: v })}
						/>
					</SettingsRow>
				</div>
				<hr />
				<Axioms />
			</div>
			<div className={"grid " + (tab == 1 ? "" : "d-none")}>
				<SettingsRow label="Use divisions" help={t("help.useDivision")}>
					<Checkbox
						className="col-form-label"
						value={mustUseDivision || tempDb.useDivision}
						disabled={mustUseDivision}
						onInput={v => setTempDb({ ...tempDb, useDivision: v })}
					/>
				</SettingsRow>
				<SettingsRow label="X divisions" help={t("help.divisions")}>
					<IntInput
						value={tempDb.numX}
						min={100}
						max={MAX_DIV}
						disabled={disableDivisions}
						onInput={v => setTempDb({ ...tempDb, numX: v })}
					/>
				</SettingsRow>
				<SettingsRow label="Y divisions" help={t("help.divisions")}>
					<IntInput
						value={tempDb.numY}
						min={100}
						max={MAX_DIV}
						disabled={disableDivisions}
						onInput={v => setTempDb({ ...tempDb, numY: v })}
					/>
				</SettingsRow>
				<SettingsRow label="Angle divisions" help={t("help.lineDivisions")}>
					<IntInput
						value={tempDb.numA}
						min={100}
						max={MAX_DIV}
						disabled={disableDivisions}
						onInput={v => setTempDb({ ...tempDb, numA: v })}
					/>
				</SettingsRow>
				<SettingsRow label="Radial divisions" help={t("help.lineDivisions")}>
					<IntInput
						value={tempDb.numD}
						min={100}
						max={MAX_DIV}
						disabled={disableDivisions}
						onInput={v => setTempDb({ ...tempDb, numD: v })}
					/>
				</SettingsRow>
				<SettingsRow label="Min aspect ratio" help={t("help.minAspectRatio")}>
					<ExpInput
						value={tempDb.minAspectRatio}
						exp={tempDb.minAspectRatioExp}
						onInput={(v, exp) => setTempDb({ ...tempDb, minAspectRatio: v, minAspectRatioExp: exp })}
					/>
				</SettingsRow>
				<SettingsRow label="Min angle sine" help={t("help.minAngle")}>
					<ExpInput
						value={tempDb.minAngleSine}
						exp={tempDb.minAngleSineExp}
						onInput={(v, exp) => setTempDb({ ...tempDb, minAngleSine: v, minAngleSineExp: exp })}
					/>
				</SettingsRow>
				<SettingsRow label="Visibility matters" help={t("help.visibility")}>
					<Checkbox
						className="col-form-label"
						value={tempDb.visibility}
						onInput={v => setTempDb({ ...tempDb, visibility: v })}
					/>
				</SettingsRow>
				<div className="mt-n2" style={{ gridColumn: "1/3" }}><hr /></div>
				<SettingsRow label="Line error mode" help={t("help.lineError")}>
					<select
						className="form-select"
						value={settings.worstCaseError ? "1" : ""}
						onChange={e => useSettings.setState({ worstCaseError: Boolean(e.currentTarget.value) })}
					>
						<option value="1">Worst-case error</option>
						<option value="">Pythagorean error</option>
					</select>
				</SettingsRow>
				<SettingsRow label="Good enough error" help={t("help.goodEnoughError")}>
					<ExpInput
						value={settings.error}
						exp={settings.errorExp}
						onInput={(v, exp) => useSettings.setState({ error: v, errorExp: exp })}
					/>
				</SettingsRow>
				<SettingsRow label="Solution count">
					<IntInput
						min={1}
						max={20}
						value={settings.count}
						onInput={v => useSettings.setState({ count: v })}
					/>
				</SettingsRow>
			</div>
			<div className={(tab == 2 ? "" : "d-none")}>
				<div className="grid">
					<SettingsRow label={t("settings.system.theme._")}>
						<select
							value={settings.theme}
							className="form-select"
							onChange={e => useSettings.setState({ theme: Number(e.currentTarget.value) })}
						>
							<option value={Theme.system}>{t("settings.system.theme.system")}</option>
							<option value={Theme.light}>{t("settings.system.theme.light")}</option>
							<option value={Theme.dark}>{t("settings.system.theme.dark")}</option>
							<option value={Theme.rabbit}>RabbitEar</option>
						</select>
					</SettingsRow>
					<SettingsRow label={t("settings.system.precision")}>
						<IntInput
							min={1}
							max={6}
							value={settings.precision}
							onInput={v => useSettings.setState({ precision: v })}
						/>
					</SettingsRow>
				</div>
				<div className="mt-3 mb-1">
					<Checkbox
						disabled={!idbSupported}
						value={settings.useDB && idbSupported}
						onInput={v => useSettings.setState({ useDB: v })}
					>{t("settings.system.useDB")}</Checkbox>
					<Checkbox
						value={settings.showInit}
						onInput={v => useSettings.setState({ showInit: v })}
					>{t("settings.system.showInit")}</Checkbox>
					<Checkbox
						value={settings.showAxiom}
						onInput={v => useSettings.setState({ showAxiom: v })}
					>{t("settings.system.showAxiom")}</Checkbox>
				</div>
			</div>
		</div>
	);
}

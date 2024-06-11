import { Suspense, lazy, useRef, useState } from "react";
import { useTranslation } from "react-i18next";

import { defaultDB, defaultSettings, useDB, useSettings } from "../../store";
import { resetWorker } from "../../bridge";
import { SettingsContext } from "./context";

import "./settings.scss";
import "./axioms.scss";
import "../form/checkbox.scss";
import { InfoTooltip } from "../tooltip";

const SettingsBody = lazy(() => import("./settingsBody"));

function Loading() {
	return <div className="modal-body p-5">
		<div className="display-4 text-center">
			<i className="fa-solid fa-spinner fa-spin"></i>
		</div>
	</div>;
}

export function Settings() {
	const { t } = useTranslation();
	const db = useDB();
	const settings = useSettings();
	const [open, setOpen] = useState(false);
	const [cacheSettings, cacheTempSettings] = useState(structuredClone(settings));
	const [tempDb, setTempDb] = useState(structuredClone(db));
	const ref = useRef(null);
	const handleShow = async () => {
		setOpen(true);
		gtag("event", "ref_show_settings");
		cacheTempSettings(structuredClone(settings));
		setTempDb(structuredClone(db));
		const bs = await import("bootstrap");
		bs.Modal.getOrCreateInstance(ref.current!, { backdrop: "static" }).show();
	};
	const handleSave = () => {
		if(hasChanged(db, tempDb)) {
			useDB.setState(tempDb);
			resetWorker(tempDb);
		}
	};
	const handleCancel = () => {
		useSettings.setState(cacheSettings);
	};

	const reset = () => {
		// Settings are written in-place, so that it can be reflected on UI immediately
		useSettings.setState(structuredClone(defaultSettings));
		setTempDb(structuredClone(defaultDB));
	};

	return <>
		<button type="button" className="btn btn-secondary" onClick={handleShow}>
			<i className="fa-solid fa-gear"></i>
			<span className="d-none d-sm-inline-block">
				&nbsp;<span className="capitalize">{t("phrase.settings")}</span>
			</span>
		</button>
		<div className="modal fade" ref={ref}>
			<div className="modal-dialog modal-dialog-centered">
				<div className="modal-content">
					{open &&
						<SettingsContext.Provider value={{ tempDb, setTempDb }}>
							<Suspense fallback={<Loading />}>
								<SettingsBody />
							</Suspense>
						</SettingsContext.Provider>
					}
					<div className="modal-footer">
						<div className="flex-grow-1">
							<button type="button" className="btn btn-secondary" onClick={reset}>{t("settings.reset")}</button>
						</div>
						<div>
							{hasChanged(db, tempDb) &&
								<InfoTooltip
									title={t("settings.reInit")}
									className="fa-solid fa-triangle-exclamation text-warning me-3"
								/>
							}
							<button
								type="button"
								className="btn btn-secondary me-2 capitalize"
								data-bs-dismiss="modal"
								onClick={handleCancel}
							>{t("keyword.cancel")}</button>
							<button
								type="button"
								className="btn btn-primary"
								data-bs-dismiss="modal"
								onClick={handleSave}
							>{t("keyword.ok")}</button>
						</div>
					</div>
				</div>
			</div>
		</div>
	</>;
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

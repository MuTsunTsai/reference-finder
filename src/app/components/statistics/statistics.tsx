import { Suspense, lazy, useRef, useState } from "react";
import { useTranslation } from "react-i18next";
import { createPortal } from "react-dom";

import { startStatistics, useWorker } from "../../bridge";
import { SettingsRow } from "../form/settings-row";
import { useSettings, useStore } from "../../store";
import { IntInput } from "../form/int-input";
import { InfoTooltip } from "../tooltip";

import "./statistics.scss";

import type { Modal } from "bootstrap";
import type { StatData } from "./chart";

const StatReport = lazy(() => import("./statReport"));

let startTime: number;
let modal: Modal;
const tempData: number[] = [];

export function Statistics() {
	const { t } = useTranslation();
	const [open, setOpen] = useState(false);
	const [data, setData] = useState<StatData | null>(null);
	const [progress, setProgress] = useState(-1);
	const [canceling, setCanceling] = useState(false);
	const store = useStore();
	const settings = useSettings();
	const ref = useRef(null);

	const handleShow = async () => {
		setOpen(true);
		const Modal = (await import("bootstrap/js/dist/modal")).default;
		modal = Modal.getOrCreateInstance(ref.current!, { backdrop: "static" });
		modal.show();
	};

	function run() {
		gtag("event", "ref_statistics_run");
		startTime = performance.now();
		setData(null);
		setCanceling(false);
		tempData.length = 0;
		setProgress(-1);
		useStore.setState({ statisticsRunning: true });
		startStatistics(settings.trials, callback);
	}

	function callback(text: string | Error) {
		if(text instanceof Error) {
			useStore.setState({ statisticsRunning: false });
			modal.hide();
			return;
		}

		const json = JSON.parse(text);
		tempData.push(...json.progress);
		setProgress(tempData.length);
		if(json.done) {
			setData({
				data: tempData.sort((a, b) => a - b),
				time: performance.now() - startTime,
			});
			useStore.setState({ statisticsRunning: false });
		}
	}

	const percentage = progress < 0 ? 0 : progress / settings.trials * 100;

	function cancel() {
		gtag("event", "ref_statistics_cancel");
		setCanceling(true);
		useWorker().postMessage("cancel");
	}

	return (
		<>
			<button type="button" className="btn btn-secondary" onClick={handleShow}>
				<i className="fa-solid fa-chart-simple" />
				<span className="d-none d-sm-inline-block">&nbsp;{t("statistics._")}</span>
			</button>
			{createPortal(
				<div className="modal fade" ref={ref}>
					<div className="modal-dialog modal-dialog-centered">
						<div className="modal-content">
							<div className="modal-header">
								{t("statistics._")}
								<InfoTooltip title={t("help.statistics")} />
							</div>
							<div className="modal-body">
								<div className="grid">
									<SettingsRow label={store.statisticsRunning ?
										(progress < 0 ? "Initializing..." : `${progress} / ${settings.trials}`) :
										t("statistics.trials")}
									>
										<div className="row gx-3 align-items-center">
											{store.statisticsRunning ?
												<>
													<div className="col">
														<div className="progress">
															<div
																className="progress-bar progress-bar-striped progress-bar-animated"
																style={{ width: percentage + "%" }}
															>
																{Math.floor(percentage)}%
															</div>
														</div>
													</div>
													<div className="col col-auto">
														<button
															className="btn btn-secondary capitalize"
															type="button"
															onClick={cancel}
															disabled={canceling}
														>
															{t("keyword.cancel")}&nbsp;<i className="fa-solid fa-spinner fa-spin"></i>
														</button>
													</div>
												</> :
												<>
													<div className="col">
														<IntInput
															value={settings.trials}
															min={1}
															onInput={v => useSettings.setState({ trials: v })}
														/>
													</div>
													<div className="col col-auto">
														<button
															className="btn btn-primary"
															type="button"
															onClick={run}
															disabled={!store.ready || store.statisticsRunning}
														>
															{!store.ready ?
																<span className="capitalize">
																	{t("phrase.initializing")}&nbsp;<i className="fa-solid fa-spinner fa-spin"></i>
																</span> :
																<><i className="fa-solid fa-play"></i>&nbsp;<span className="capitalize">{t("phrase.go")}</span></>
															}
														</button>
													</div>
												</>
											}
										</div>
									</SettingsRow>
								</div>
								{open &&
									<Suspense>
										<StatReport data={data} />
									</Suspense>
								}
							</div>
							<div className="modal-footer">
								<button type="button" className="btn btn-secondary" data-bs-dismiss="modal" disabled={store.statisticsRunning}>{t("keyword.ok")}</button>
							</div>
						</div>
					</div>
				</div>,
				document.body
			)}
		</>
	);
}

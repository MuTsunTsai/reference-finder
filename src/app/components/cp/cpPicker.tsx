import { useEffect, useRef, useState } from "react";
import { createPortal } from "react-dom";
import { useTranslation } from "react-i18next";

import { getSelectionPoints } from "../../cp/parse";
import { getTargetError } from "../../restriction";
import { useDB, useSettings, useStore } from "../../store";
import { CpViewer } from "./cpViewer";
import { useDescribeSelection } from "./describe";

import type { CpSelection, FittedPattern } from "../../cp/parse";

interface CpPickerProps {
	cp: FittedPattern;
	selection: CpSelection | null;
	onConfirm: (selection: CpSelection) => void;
}

/**
 * The button that opens the modal for selecting
 * a point or a line from the crease pattern.
 */
export function CpPicker({ cp, selection, onConfirm }: CpPickerProps) {
	const { t } = useTranslation();
	const db = useDB();
	const settings = useSettings();
	const { existingLines } = useStore();
	const describe = useDescribeSelection(cp);
	const ref = useRef<HTMLDivElement>(null);
	const [open, setOpen] = useState(false);
	const [temp, setTemp] = useState<CpSelection | null>(selection);
	const [hover, setHover] = useState<CpSelection | null>(null);
	const [fitKey, setFitKey] = useState(0);

	// The size of the viewer is only known after the modal is shown
	useEffect(() => {
		const el = ref.current;
		if(!el) return;
		const handler = () => setFitKey(k => k + 1);
		el.addEventListener("shown.bs.modal", handler);
		return () => el.removeEventListener("shown.bs.modal", handler);
	}, []);

	const show = async () => {
		gtag("event", "ref_show_cp_picker");
		setTemp(selection);
		setHover(null);
		setOpen(true);
		const Modal = (await import("bootstrap/js/dist/modal")).default;
		Modal.getOrCreateInstance(ref.current!).show();
	};

	const errorKey = temp && getTargetError(
		getSelectionPoints(cp, temp), db, existingLines, settings.error
	);

	return <>
		<button type="button" className="btn btn-primary" onClick={show}>
			<i className="fa-solid fa-arrow-pointer"></i>&nbsp;<span className="capitalize">{t("cp.select")}</span>
		</button>
		{createPortal(
			<div className="modal fade" ref={ref}>
				<div className="modal-dialog modal-xl modal-dialog-centered modal-fullscreen-lg-down">
					<div className="modal-content">
						<div className="modal-header">
							<div>
								<h5 className="modal-title">{t("cp.title")}</h5>
								<div className="small text-muted">{t("cp.hint")}</div>
							</div>
							<button type="button" className="btn-close" data-bs-dismiss="modal" />
						</div>
						<div className="modal-body p-0 position-relative" style={{ height: "70vh" }}>
							{open &&
								<CpViewer
									cp={cp}
									width={db.width}
									height={db.height}
									selection={temp}
									fitKey={fitKey}
									onSelect={setTemp}
									onHover={setHover}
								/>
							}
							<button
								type="button"
								className="btn btn-sm btn-secondary position-absolute top-0 end-0 m-2"
								onClick={() => setFitKey(k => k + 1)}
							>
								<i className="fa-solid fa-compress"></i>&nbsp;{t("cp.fit")}
							</button>
						</div>
						<div className="modal-footer">
							<div className="flex-grow-1">
								<div>{describe(hover ?? temp) ?? t("cp.none")}</div>
								{errorKey && <div className="text-warning small">{t(errorKey)}</div>}
							</div>
							<button type="button" className="btn btn-secondary capitalize" data-bs-dismiss="modal">
								{t("keyword.cancel")}
							</button>
							<button
								type="button"
								className="btn btn-primary"
								data-bs-dismiss="modal"
								disabled={!temp || Boolean(errorKey)}
								onClick={() => temp && onConfirm(temp)}
							>{t("keyword.ok")}</button>
						</div>
					</div>
				</div>
			</div>,
			document.body
		)}
	</>;
}

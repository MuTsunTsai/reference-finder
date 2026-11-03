import { useEffect, useRef } from "react";
import { createPortal } from "react-dom";
import { useTranslation } from "react-i18next";

import { getAspectRatio } from "../../cp/parse";
import { useDB, useSettings } from "../../store";

import type { CreasePattern } from "../../cp/parse";

interface AspectConfirmProps {
	/** The pattern waiting for confirmation; the dialog is shown when this is set. */
	cp: CreasePattern | null;
	onAnswer: (yes: boolean) => void;
}

/** Asks whether to change the sheet dimensions to match the aspect ratio of the opened pattern. */
export function AspectConfirm({ cp, onAnswer }: AspectConfirmProps) {
	const { t } = useTranslation();
	const db = useDB();
	const { precision } = useSettings();
	const ref = useRef<HTMLDivElement>(null);
	const answered = useRef(false);

	useEffect(() => {
		if(!cp) return;
		answered.current = false;
		(async () => {
			const Modal = (await import("bootstrap/js/dist/modal")).default;
			Modal.getOrCreateInstance(ref.current!, { backdrop: "static" }).show();
		})();
	}, [cp]);

	// Closing the dialog in any other way counts as "no"
	useEffect(() => {
		const el = ref.current;
		if(!el) return;
		const handler = () => {
			if(!answered.current) onAnswer(false);
		};
		el.addEventListener("hidden.bs.modal", handler);
		return () => el.removeEventListener("hidden.bs.modal", handler);
	}, [onAnswer]);

	function answer(yes: boolean) {
		answered.current = true;
		onAnswer(yes);
	}

	function ratio(r: number): string {
		return r.toFixed(precision).replace(/\.?0+$/, "") + " : 1";
	}

	return createPortal(
		<div className="modal fade" ref={ref}>
			<div className="modal-dialog modal-dialog-centered">
				<div className="modal-content">
					<div className="modal-body">
						{cp && t("cp.aspectConfirm", { cp: ratio(getAspectRatio(cp)), sheet: ratio(db.width / db.height) })}
					</div>
					<div className="modal-footer">
						<button
							type="button"
							className="btn btn-secondary capitalize"
							data-bs-dismiss="modal"
							onClick={() => answer(false)}
						>{t("keyword.no")}</button>
						<button
							type="button"
							className="btn btn-primary capitalize"
							data-bs-dismiss="modal"
							onClick={() => answer(true)}
						>{t("keyword.yes")}</button>
					</div>
				</div>
			</div>
		</div>,
		document.body
	);
}

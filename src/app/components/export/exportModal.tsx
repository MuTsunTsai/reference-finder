import { useEffect, useRef } from "react";
import { createPortal } from "react-dom";
import { useTranslation } from "react-i18next";

import type Modal from "bootstrap/js/dist/modal";
import type { ExportFormat } from "../../export/fileSystem";

interface ExportModalProps {
	blob: Blob | null;
	format: ExportFormat | null;
	defaultName: string;
	onClose: () => void;
}

export function ExportModal({ blob, format, defaultName, onClose }: ExportModalProps) {
	const { t } = useTranslation();
	const ref = useRef<HTMLDivElement>(null);
	const inputRef = useRef<HTMLInputElement>(null);
	const linkRef = useRef<HTMLAnchorElement>(null);

	useEffect(() => {
		if(!blob || !format || !ref.current) return;
		let modal: Modal | undefined;
		(async () => {
			const Modal = (await import("bootstrap/js/dist/modal")).default;
			modal = Modal.getOrCreateInstance(ref.current!);
			modal.show();
		})();
	}, [blob, format]);

	useEffect(() => {
		const el = ref.current;
		if(!el) return;
		const handler = () => {
			if(objectUrl) URL.revokeObjectURL(objectUrl);
			onClose();
		};
		el.addEventListener("hidden.bs.modal", handler);
		return () => el.removeEventListener("hidden.bs.modal", handler);
	});

	const objectUrl = blob ? URL.createObjectURL(blob) : null;

	const handleDownload = async () => {
		if(!linkRef.current || !inputRef.current || !objectUrl || !format || !ref.current) return;
		const name = inputRef.current.value.trim() || defaultName;
		linkRef.current.href = objectUrl;
		linkRef.current.download = name + "." + format;
		linkRef.current.click();
		const { default: Modal } = await import("bootstrap/js/dist/modal");
		Modal.getOrCreateInstance(ref.current).hide();
	};

	return createPortal(
		<div className="modal fade" ref={ref}>
			<div className="modal-dialog modal-dialog-centered">
				<div className="modal-content">
					<div className="modal-header">
						<h5 className="modal-title capitalize">{t("phrase.export")}</h5>
						<button type="button" className="btn-close" data-bs-dismiss="modal" />
					</div>
					<div className="modal-body">
						<div className="input-group">
							<input
								ref={inputRef}
								type="text"
								className="form-control"
								defaultValue={defaultName}
							/>
							<span className="input-group-text">{format ? "." + format : ""}</span>
							<button type="button" className="btn btn-primary capitalize" onClick={handleDownload}>
								{t("keyword.download")}
							</button>
						</div>
					</div>
				</div>
			</div>
			{/* Hidden anchor for triggering download */}
			<a ref={linkRef} className="d-none" />
		</div>,
		document.body
	);
}

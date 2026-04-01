import { useRef, useState } from "react";
import { useTranslation } from "react-i18next";

import { useSettings } from "../../store";
import { formatSolution } from "../formatSolution";
import { ExportModal } from "./exportModal";

import type { RefObject } from "react";
import type { Solution } from "../../store";
import type { ExportFormat } from "../../export/fileSystem";

interface ExportButtonProps {
	cardBodyRef: RefObject<HTMLDivElement | null>;
	solution: Solution;
}

export function ExportButton({ cardBodyRef, solution }: ExportButtonProps) {
	const { t } = useTranslation();
	const settings = useSettings();
	const [loading, setLoading] = useState(false);
	const [exportState, setExportState] = useState<{ blob: Blob, format: ExportFormat, name: string } | null>(null);
	const toggleRef = useRef<HTMLButtonElement>(null);

	const handleExport = async (format: ExportFormat) => {
		// Close the dropdown synchronously before starting the async export
		if(toggleRef.current) {
			const { default: Dropdown } = await import("bootstrap/js/dist/dropdown");
			Dropdown.getOrCreateInstance(toggleRef.current).hide();
		}
		if(!cardBodyRef.current || loading) return;
		setLoading(true);
		try {
			const { exportAsPng, exportAsSvg, exportAsPdf } = await import("../../export/exportUtils");
			let blob: Blob;
			if(format === "png") blob = await exportAsPng(cardBodyRef.current);
			else if(format === "svg") blob = await exportAsSvg(cardBodyRef.current);
			else blob = await exportAsPdf(cardBodyRef.current);
			const name = "solution-" + formatSolution(solution, settings.precision).replace(/[^0-9.,-]/g, "");
			const { saveWithPicker } = await import("../../export/fileSystem");
			await saveWithPicker(blob, format, name);
		} catch(e) {
			console.error(e);
		} finally {
			setLoading(false);
		}
	};

	return (
		<>
			<div className="btn-group">
				<button
					ref={toggleRef}
					type="button"
					className="btn btn-secondary py-0 dropdown-toggle ms-2"
					data-bs-toggle="dropdown"
					aria-expanded="false"
					disabled={loading}
				>
					{loading ?
						<i className="fa-solid fa-spinner fa-spin"></i> :
						<i className="fa-solid fa-file-export"></i>
					}
					<span className="d-inline-block capitalize m-1">&nbsp;{t("phrase.export")}</span>
				</button>
				<ul className="dropdown-menu dropdown-menu-end" style={{ minWidth: "unset" }}>
					<li><button type="button" className="dropdown-item" onClick={() => handleExport("png")}>PNG</button></li>
					<li><button type="button" className="dropdown-item" onClick={() => handleExport("svg")}>SVG</button></li>
					<li><button type="button" className="dropdown-item" onClick={() => handleExport("pdf")}>PDF</button></li>
				</ul>
			</div>
			{exportState &&
				<ExportModal
					blob={exportState.blob}
					format={exportState.format}
					defaultName={exportState.name}
					onClose={() => setExportState(null)}
				/>
			}
		</>
	);
}

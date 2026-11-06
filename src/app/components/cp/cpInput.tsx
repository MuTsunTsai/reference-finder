import { useCallback, useRef, useState } from "react";
import { useTranslation } from "react-i18next";

import { CP_ACCEPT, hasOpenFilePicker, pickCpFile } from "../../cp/openFile";
import { getAspectRatio, matchesAspectRatio, parseCp } from "../../cp/parse";
import { resetWorker } from "../../bridge";
import { useDB } from "../../store";
import { AspectConfirm } from "./aspectConfirm";
import { CpPicker } from "./cpPicker";
import { useDescribeSelection } from "./describe";

import type { CpSelection, CreasePattern, FittedPattern } from "../../cp/parse";

import "./cpInput.scss";

interface CpInputProps {
	cp: CreasePattern | null;
	fitted: FittedPattern | null;
	selection: CpSelection | null;
	/** Error message for the current selection. */
	error?: string | null;
	onLoad: (cp: CreasePattern) => void;
	onSelect: (selection: CpSelection) => void;
}

const EXP_DIGITS = 10;

/**
 * The area for opening a crease pattern file
 * and selecting the target from it.
 */
export function CpInput(
	{ cp, fitted, selection, error, onLoad, onSelect }: CpInputProps
) {
	const { t } = useTranslation();
	const describe = useDescribeSelection(fitted);
	const inputRef = useRef<HTMLInputElement>(null);
	const [dragging, setDragging] = useState(false);
	const [loadError, setLoadError] = useState<string | null>(null);
	/**
	 * A pattern whose aspect ratio differs from the sheet,
	 * waiting for the confirmation.
	 */
	const [pending, setPending] = useState<CreasePattern | null>(null);

	async function load(file: File) {
		try {
			const result = parseCp(file.name, await file.text());
			gtag("event", "ref_load_cp");
			setLoadError(null);
			const { width, height } = useDB.getState();
			if(matchesAspectRatio(result, width, height)) onLoad(result);
			else setPending(result);
		} catch(e) {
			setLoadError(t("cp.loadError", {
				message: (e as Error).message,
				interpolation: { escapeValue: false },
			}));
		}
	}

	/**
	 * If confirmed, changes the sheet dimensions to match the pending pattern;
	 * otherwise the opening is cancelled.
	 */
	const answerAspect = useCallback((yes: boolean) => {
		setPending(null);
		if(!yes || !pending) return;
		const db = useDB.getState();
		const height = db.width / getAspectRatio(pending);
		const heightExp = String(Number(height.toPrecision(EXP_DIGITS)));
		const newDb = { ...db, height, heightExp };
		useDB.setState(newDb);
		resetWorker(newDb);
		onLoad(pending);
	}, [pending, onLoad]);

	async function open() {
		if(!hasOpenFilePicker) {
			inputRef.current!.click();
			return;
		}
		try {
			const file = await pickCpFile();
			if(file) await load(file);
		} catch(e) {
			console.error(e);
			// Fallback, e.g. when the API is blocked in an iframe
			inputRef.current!.click();
		}
	}

	function drop(e: React.DragEvent) {
		e.preventDefault();
		setDragging(false);
		const file = e.dataTransfer.files[0];
		if(file) load(file);
	}

	return (
		<div className="mt-2">
			<div className="row g-2">
				<div className="col-12 col-sm">
					<div
						className={"cp-drop h-100 rounded p-2 text-center" + (dragging ? " dragging" : "")}
						role="button"
						tabIndex={0}
						onClick={open}
						onKeyDown={e => (e.key == "Enter" || e.key == " ") && open()}
						onDragOver={e => {
							e.preventDefault();
							setDragging(true);
						}}
						onDragLeave={() => setDragging(false)}
						onDrop={drop}
					>
						{/*
							Both states have the same layout,
							so that the height doesn't change after opening
						*/}
						{cp ?
							<>
								<i className="fa-solid fa-file-lines"></i>&nbsp;
								<span className="fw-bold">{cp.name}</span>
								<div className="small text-muted">{t("cp.change")}</div>
							</> :
							<>
								<i className="fa-solid fa-file-import"></i>&nbsp;
								<span className="fw-bold">{t("cp.open")}</span>
								<div className="small text-muted">{t("cp.formats")}</div>
							</>
						}
					</div>
				</div>
				{fitted &&
					<div className="col-12 col-sm d-flex align-items-center gap-2">
						<CpPicker
							cp={fitted}
							selection={selection}
							onConfirm={onSelect}
						/>
						<span className={selection ? "" : "text-muted"}>
							{describe(selection) ?? t("cp.none")}
						</span>
					</div>
				}
			</div>
			<input
				ref={inputRef}
				type="file"
				accept={CP_ACCEPT}
				className="d-none"
				onChange={e => {
					const file = e.currentTarget.files?.[0];
					if(file) load(file);
					e.currentTarget.value = ""; // Allow opening the same file again
				}}
			/>
			{loadError && <div className="text-danger small mt-1">{loadError}</div>}
			{error && <div className="text-danger small mt-1">{error}</div>}
			{fitted?.aspectMismatch &&
				<div className="text-warning small mt-1">{t("cp.aspect")}</div>
			}
			<AspectConfirm cp={pending} onAnswer={answerAspect} />
		</div>
	);
}

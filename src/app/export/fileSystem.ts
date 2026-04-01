export type ExportFormat = "png" | "svg" | "pdf";

const MIME: Record<ExportFormat, Record<MIMEType, FileExtension>> = {
	png: {
		"image/png": ".png",
	},
	svg: {
		"image/svg+xml": ".svg",
	},
	pdf: {
		"application/pdf": ".pdf",
	},
};

const EXT: Record<ExportFormat, string> = {
	png: ".png",
	svg: ".svg",
	pdf: ".pdf",
};

/** Whether the browser supports the File System Access API. */
export const hasFileSystemAccess: boolean =
	typeof window !== "undefined" && typeof (window as Window & { showSaveFilePicker?: unknown }).showSaveFilePicker === "function";

/**
 * If the browser supports File System Access API, shows a save dialog and writes the blob.
 * Returns false if the user cancels or the API is not supported (caller should fall back to modal).
 */
export async function saveWithPicker(blob: Blob, format: ExportFormat, suggestedName: string): Promise<boolean> {
	if(!hasFileSystemAccess) return false;
	try {
		const handle = await (window.showSaveFilePicker({
			suggestedName: suggestedName + EXT[format],
			types: [{ description: format.toUpperCase(), accept: MIME[format] }],
		}));
		const writable = await handle.createWritable();
		await writable.write(blob);
		await writable.close();
		return true;
	} catch(e) {
		if((e as DOMException).name !== "AbortError") console.error(e);
		return false;
	}
}

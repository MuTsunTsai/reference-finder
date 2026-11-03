/** File extensions accepted for crease patterns. */
export const CP_ACCEPT = ".cp,.fold";

/** Whether the browser supports opening files with the File System Access API. */
export const hasOpenFilePicker: boolean =
	typeof window !== "undefined" && typeof (window as Window & { showOpenFilePicker?: unknown }).showOpenFilePicker === "function";

/**
 * Shows the open dialog of the File System Access API.
 * Returns null if the user cancels.
 */
export async function pickCpFile(): Promise<File | null> {
	try {
		const [handle] = await window.showOpenFilePicker({
			types: [{
				description: "Crease pattern",
				accept: {
					"text/plain": [".cp"],
					"application/json": [".fold"],
				},
			}],
		});
		return await handle.getFile();
	} catch(e) {
		if((e as DOMException).name === "AbortError") return null;
		throw e;
	}
}

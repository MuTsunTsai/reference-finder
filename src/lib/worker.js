import ref from "./ref";

const wasmURL = new URL("./ref.wasm", import.meta.url);

// We use this entry script to hook up with rsbuild (formerly webpack)
// See https://gist.github.com/surma/b2705b6cca29357ebea1c9e6e15684cc
ref({
	locateFile(path) {
		if(path.endsWith(".wasm")) return wasmURL.pathname;
		return path;
	}
});

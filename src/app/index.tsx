import React from "react";
import ReactDOM from "react-dom/client";
import App from "./app";
import structuredClone from "@ungap/structured-clone";

import "./locale";
import "./style.scss";

// Global polyfill
if(!("structuredClone" in globalThis)) {
	globalThis.structuredClone = structuredClone as typeof globalThis["structuredClone"];
}

// Initialize
const root = ReactDOM.createRoot(
	document.getElementById("root") as HTMLElement
);

root.render(
	<React.StrictMode>
		<App />
	</React.StrictMode>
);

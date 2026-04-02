import { readFileSync } from "fs";
import { dirname, resolve } from "path";
import { fileURLToPath } from "url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const localeDir = resolve(__dirname, "../src/locale");

// Keys that appear in exported solution step text
const STEP_KEYS = [
	"axiom",
	"axiom_solution",
	"intersection",
	"intersection_solution",
	"pinch",
	"paper",
	"phrase.point",
	"phrase.line",
];

// Locale files that need custom font subsetting (non-Latin scripts)
const LOCALES = ["zh-TW.json", "zh-CN.json", "ja.json", "ru.json"];

function extractValues(obj, key) {
	const parts = key.split(".");
	let current = obj;
	for(const part of parts) {
		if(current == null) return [];
		current = current[part];
	}
	if(current == null) return [];
	if(typeof current === "string") return [current];
	if(Array.isArray(current)) return current;
	if(typeof current === "object") {
		return Object.values(current).flatMap(v => {
			if(typeof v === "string") return [v];
			if(Array.isArray(v)) return v;
			return [];
		});
	}
	return [];
}

function* nonAscii(str) {
	for(const ch of str) {
		const cp = ch.codePointAt(0);
		if(cp > 0x7F) yield ch;
	}
}

const allChars = new Set();

const FIXED = "1234567890. ABCDEFGHIJKLMNOPQRSTUVWXYZ";

for(const file of LOCALES) {
	const chars = new Set();
	const json = JSON.parse(readFileSync(resolve(localeDir, file), "utf-8"));
	for(const key of STEP_KEYS) {
		const values = extractValues(json, key);
		for(const str of values) {
			for(const ch of nonAscii(str)) {
				chars.add(ch);
				allChars.add(ch);
			}
		}
	}
	const sorted = [...chars].sort((a, b) => a.codePointAt(0) - b.codePointAt(0));
	console.log(`\n${file} (${sorted.length} chars):`);
	console.log(FIXED + sorted.join(""));
}

const allSorted = [...allChars].sort((a, b) => a.codePointAt(0) - b.codePointAt(0));
console.log(`\nTotal unique: ${FIXED}${allSorted.length}`);
console.log(allSorted.join(""));

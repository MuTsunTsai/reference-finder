import i18n from "i18next";
import detector from "i18next-browser-languagedetector";
import { initReactI18next } from "react-i18next";
import probablyChina from "probably-china";

import en from "../locale/en.json";
import es from "../locale/es.json";
import pt from "../locale/pt.json";
import ru from "../locale/ru.json";
import ja from "../locale/ja.json";
import cn from "../locale/zh-CN.json";
import tw from "../locale/zh-TW.json";

if(probablyChina) {
	// Replace the flag to avoid unnecessary trouble.
	tw.name = "繁體中文";
	tw.flag = "🇭🇰";
}

const resources = {
	"en": { translation: en },
	"es": { translation: es },
	"ja": { translation: ja },
	"pt": { translation: pt },
	"ru": { translation: ru },
	"zh-CN": { translation: cn },
	"zh-TW": { translation: tw },
};

export const supportedLngs = Object.keys(resources);

// localStorage is shared by all apps on mutsuntsai.github.io, so use a prefixed key
// instead of the default "i18nextLng".
const LNG_KEY = "reference-finder:lng";

/**
 * Moves the language from the default key of i18next-browser-languagedetector.
 *
 * TODO: Migration code; can be removed after 2027-10.
 */
function migrateLanguage(): void {
	try {
		const lng = localStorage.getItem("i18nextLng");
		if(lng === null) return;
		if(localStorage.getItem(LNG_KEY) === null) localStorage.setItem(LNG_KEY, lng);
		localStorage.removeItem("i18nextLng");
	} catch {
		// Ignore unavailable storage
	}
}
migrateLanguage();

i18n
	.use(detector)
	.use(initReactI18next)
	.init({
		resources,
		fallbackLng: "en",
		supportedLngs,
		detection: {
			order: ["querystring", "localStorage", "navigator"],
			caches: ["localStorage"],
			lookupLocalStorage: LNG_KEY,
		},
	});

export { i18n };

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
		},
	});

export { i18n };

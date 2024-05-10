import i18n from "i18next";
import detector from "i18next-browser-languagedetector";
import { initReactI18next } from "react-i18next";

import en from "../locale/en.json";
import es from "../locale/es.json";
import pt from "../locale/pt.json";
import ru from "../locale/ru.json";
import ja from "../locale/ja.json";
import cn from "../locale/zh-CN.json";
import tw from "../locale/zh-TW.json";

const resources = {
	en: { translation: en },
	es: { translation: es },
	ja: { translation: ja },
	pt: { translation: pt },
	ru: { translation: ru },
	"zh-CN": { translation: cn },
	"zh-TW": { translation: tw },
};

/**
 * A lucky guess that we are probably in China.
 * If we are, replace the flag to avoid unnecessary trouble.
 */
const detectChinaLanguage = ["zh-CN", "zh-CHS", "zh-Hans", "zh-HK", "zh-MO"];
const detectChinaTimeZone = ["Asia/Shanghai", "Asia/Urumqi"];
const timeZone = new Intl.DateTimeFormat().resolvedOptions().timeZone;
const isChina =
	detectChinaTimeZone.includes(timeZone) ||
	detectChinaLanguage.includes(navigator.language) ||
	navigator.languages.some(l => detectChinaLanguage.includes(l));
if(isChina) {
	resources["zh-TW"].translation.name = "繁體中文";
	resources["zh-TW"].translation.flag = "🇭🇰";
}

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
		}
	});

export { i18n };

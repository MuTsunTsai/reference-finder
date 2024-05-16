import { useTranslation } from "react-i18next";
import { supportedLngs } from "../../locale";
import { useEffect } from "react";

export function Locale() {
	const { t, i18n } = useTranslation();

	useEffect(() => {
		document.title = t("phrase.referenceFinder");
	}, [i18n.language]);

	return (
		<select className="form-select flag d-inline-block" style={{ width: "max-content" }}
			aria-label="Language"
			value={i18n.language} onChange={e => i18n.changeLanguage(e.currentTarget.value)}>
			{supportedLngs.map(l =>
				<option key={l} value={l}>{t("flag", { lng: l })} {t("name", { lng: l })}</option>
			)}
		</select>
	);
}

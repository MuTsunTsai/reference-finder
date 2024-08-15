import { useTranslation } from "react-i18next";

declare const __VERSION__: string;

export function Footer() {
	const { t } = useTranslation();
	return (
		<div className="container mt-4 px-4">
			<footer className="row justify-content-end flex-wrap pb-3">
				<div className="col-12 col-md-auto flex-grow-1">{t("phrase.coreVer")}: <a href="https://github.com/MuTsunTsai/reference-finder/blob/main/CHANGELOG.md" target="_blank">v{__VERSION__}</a></div>
				<div className="col-12 col-md-auto">
					© 1999-2007 <a href="https://langorigami.com/article/referencefinder/" target="_blank">Robert J. Lang</a>
				</div>
				<div className="col-12 col-md-auto">
					© 2018-2022 <a href="https://github.com/mayakraft/ReferenceFinder" target="_blank">Maya Kraft</a>
				</div>
				<div className="col-12 col-md-auto">
					© 2024 <a href="https://github.com/mutsuntsai/reference-finder" target="_blank">Mu-Tsun Tsai</a>
				</div>
			</footer>
		</div>
	);
}

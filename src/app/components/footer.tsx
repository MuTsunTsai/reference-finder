import { useTranslation } from "react-i18next";

declare const __VERSION__: string;

export function Footer() {
	const { t } = useTranslation();
	return (
		<div className="container-fluid mt-4 px-4">
			<footer className="row justify-content-end flex-wrap pb-3">
				<div className="col-12 d-flex flex-wrap justify-content-between flex-column flex-lg-row">
					<div className="flex-grow-1">{t("phrase.coreVer")}: <a href="https://github.com/MuTsunTsai/reference-finder/blob/main/CHANGELOG.md" target="_blank">v{__VERSION__}</a></div>
					<div className="mx-2">
						© 1999-2007 <a href="https://langorigami.com/article/referencefinder/" target="_blank">Robert J. Lang</a>
					</div>
					<div className="mx-2">
						© 2018-2022 <a href="https://github.com/mayakraft/ReferenceFinder" target="_blank">Maya Kraft</a>
					</div>
					<div className="mx-2">
						© 2024-2025 <a href="https://github.com/mutsuntsai/reference-finder" target="_blank">Mu-Tsun Tsai</a>
					</div>
					<div className="mx-2">
						© 2024-2025 <a href="https://github.com/Omri-Shavit/reference-finder" target="_blank">Omri Shavit</a>
					</div>
				</div>
			</footer>
		</div>
	);
}

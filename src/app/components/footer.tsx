import { useTranslation } from "react-i18next";

declare const __VERSION__: string;

export function Footer() {
	const { t } = useTranslation();
	return (
		<div className="container mt-4 px-4">
			<footer className="row justify-content-end flex-wrap pb-3">
				<div className="col-12 col-lg-6">{t("phrase.coreVer")}: <a href="https://github.com/MuTsunTsai/reference-finder/blob/main/CHANGELOG.md" target="_blank">v{__VERSION__}</a></div>
				<div className="col-12 col-sm-5 col-md-4 col-lg-3">
					© 1999-2007 <a href="https://langorigami.com/article/referencefinder/" target="_blank">Robert J. Lang</a>
				</div>
				<div className="col-12 col-sm-7 col-md-8 col-lg-3">
					© 2018-2022 <a href="https://github.com/mayakraft/ReferenceFinder" target="_blank">Maya Kraft</a>
				</div>
				<div className="col-12 col-sm-5 col-md-4 col-lg-3">
					© 2024-{new Date().getFullYear()} <a href="https://github.com/mutsuntsai/reference-finder" target="_blank">Mu-Tsun Tsai</a>
				</div>
				<div className="col-12 col-sm-7 col-md-8 col-lg-3">
					© 2024-2025 <a href="https://github.com/Omri-Shavit/reference-finder" target="_blank">Omri Shavit</a>
				</div>
			</footer>
		</div>
	);
}

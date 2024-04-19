
import { useEffect, useState } from "react";
import { useTranslation } from "react-i18next";

import { Theme, useSettings, useStore } from "./store";
import { SolutionComponent } from "./components/solution";
import { supportedLngs } from "./locale";
import { Diagram } from "./components/svg/diagram";
import { Panel } from "./components/panel";

function App() {
	const { t, i18n } = useTranslation();
	const store = useStore();
	const settings = useSettings();
	const [sol, setSol] = useState<number>(0);

	const [systemDark, setSystemDark] = useState(true);
	const match = matchMedia("(prefers-color-scheme: dark)");
	match.addEventListener("change", toggleDark);
	function toggleDark() { setSystemDark(match.matches); }
	useEffect(toggleDark);

	useEffect(() => {
		let theme = "light";
		if(settings.theme == Theme.dark || settings.theme == Theme.system && systemDark) theme = "dark";
		if(settings.theme == Theme.rabbitEar) theme = "rabbit";
		document.documentElement.dataset.bsTheme = theme;
	});

	useEffect(() => {
		document.title = t("phrase.referenceFinder");
	}, [i18n.language])

	return (<>
		<div className="container px-4">
			<div className="row gx-0 py-2">
				<div className="col small text-muted">
					{store.progress && settings.showInit && (<>
						<span className="d-none d-sm-block">
							{store.ready ? "" : "Initializing"} {store.progress.lines} lines and {store.progress.marks} marks of rank ≤ {store.progress.rank}{store.ready ? " ready." : "..."}
						</span>
						<div className="d-sm-none" style={{ lineHeight: 1 }}>
							{store.progress.lines} lines<br />
							{store.progress.marks} marks{store.ready ? "" : "..."}
						</div>
					</>)}
				</div>
				<div className="col-auto">
					<select className="form-select flag d-inline-block" style={{ width: "max-content" }}
						value={i18n.language} onChange={e => i18n.changeLanguage(e.currentTarget.value)}>
						{supportedLngs.map(l => (
							<option key={l} value={l}>{t("flag", { lng: l })} {t("name", { lng: l })}</option>
						))}
					</select>
				</div>
			</div>
			<h1>{t("phrase.referenceFinder")}</h1>
			<Panel onSubmit={() => setSol(0)} />
			{store.coreError && (
				<div className="text-danger mb-3">Error: {store.coreError}</div>
			) || store.solutions.length == 0 && (!store.ready || !store.running) && (
				<div>
					<div>{t("tip.title")}</div>
					<ol>
						<li>{t("tip.origin")}</li>
						<li>{t("tip.exp", { exp: "1/sqrt(2)+sin(10)", interpolation: { escapeValue: false } })}</li>
					</ol>
				</div>
			)}
		</div>
		{store.solutions.length > 0 && (
			<section className="solutions d-none d-sm-flex">
				<div className="container px-4">
					<div className="row gx-2">
						{store.solutions.map((s, i) => (
							<div className="col mb-2" key={i} style={{ flex: "0 1 12rem" }}>
								<button className={"w-100 p-1 btn btn-ld col solution-preview " + (sol == i ? "active" : "")}
									onClick={() => setSol(i)}>
									<Diagram data={s.diagrams[s.diagrams.length - 1]} />
									<div>{s.solution}</div>
									<div>{t("phrase.error")} {s.err}</div>
									<div>rank {s.rank}</div>
								</button>
							</div>
						))}
					</div>
				</div>
			</section>
		)}
		<div className="container px-4 flex-grow-1">
			{store.solutions.map((s, i) =>
				<SolutionComponent key={i} data={s} show={sol == i} onSelect={() => setSol(i)} />
			)}
		</div>
		<div className="container mt-4 px-4">
			<footer className="row justify-content-end flex-wrap pb-3">
				<div className="col-12 col-md-auto flex-grow-1">Core version: v4.0.2</div>
				<div className="col-12 col-md-auto">
					© 1999-2007 <a href="https://langorigami.com/article/referencefinder/" target="_blank">Robert J. Lang</a>
				</div>
				<div className="col-12 col-md-auto">
					© 2018-2022 <a href="https://github.com/robbykraft/ReferenceFinder" target="_blank">Robby Kraft</a>
				</div>
				<div className="col-12 col-md-auto">
					© 2024 <a href="https://github.com/mutsuntsai/reference-finder" target="_blank">Mu-Tsun Tsai</a>
				</div>
			</footer>
		</div>
	</>);
}

export default App;

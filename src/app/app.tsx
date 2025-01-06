import { useEffect, useState } from "react";
import { useTranslation } from "react-i18next";

import { Theme, useSettings, useStore } from "./store";
import { SolutionComponent, formatSolution } from "./components/solution";
import { Diagram } from "./components/svg/diagram";
import { Panel } from "./components/panel";
import { Footer } from "./components/footer";
import { Locale } from "./components/form/locale";
import { Exact } from "./components/exact";

function App() {
	const { t } = useTranslation();
	const store = useStore();
	const settings = useSettings();
	const [sol, setSol] = useState<number>(0);

	const [systemDark, setSystemDark] = useState(true);
	const match = matchMedia("(prefers-color-scheme: dark)");
	useEffect(match.onchange = () => setSystemDark(match.matches));

	useEffect(() => {
		let theme = Theme.light;
		if(settings.theme == Theme.dark || settings.theme == Theme.system && systemDark) theme = Theme.dark;
		if(settings.theme == Theme.rabbit) theme = Theme.rabbit;
		useStore.setState({ computedTheme: theme });
		document.documentElement.dataset.bsTheme = Theme[theme];
	}, [settings.theme, systemDark]);

	return <>
		<div className="container px-4">
			<div className="row gx-0 py-2">
				<div className="col small text-muted">
					{store.progress && settings.showInit && <>
						<span className="d-none d-sm-block">
							{store.ready ? "" : "Initializing"} {store.progress.lines} lines and {store.progress.marks} marks of rank ≤ {store.progress.rank}{store.ready ? " ready." : "..."}
						</span>
						<div className="d-sm-none" style={{ lineHeight: 1 }}>
							{store.progress.lines} lines<br />
							{store.progress.marks} marks{store.ready ? "" : "..."}
						</div>
					</>}
				</div>
				<div className="col-auto">
					<Locale />
				</div>
			</div>
			<h1>{t("phrase.referenceFinder")}</h1>
			<Panel onSubmit={() => setSol(0)} />
			{store.coreError &&
				<div className="text-danger mb-3">Error: {store.coreError}</div> ||
				store.solutions.length == 0 && (!store.ready || !store.running) &&
				<div className="mb-3">
					<div>{t("tip.title")}</div>
					<ol>
						<li>{t("tip.origin")}</li>
						<li>{t("tip.exp", { exp: "1/sqrt(2)+sin(10)", interpolation: { escapeValue: false } })}</li>
					</ol>
				</div>
			}
		</div>
		<section className="solutions d-none d-sm-flex">
			{store.solutions.length > 0 &&
				<div className="container px-4">
					<div className="row gx-2">
						{store.solutions.map((s, i) =>
							<div className="col mb-2" key={i} style={{ flex: "0 1 12rem" }}>
								<button
									type="button"
									onClick={() => setSol(i)}
									className={"w-100 p-1 btn btn-ld col solution-preview " + (sol == i ? "active" : "")}
								>
									<Diagram data={s.diagrams[s.diagrams.length - 1]} padding={0.1} />
									<div>{formatSolution(s, settings.precision)}</div>
									<div><Exact err={s.err} />{t("phrase.error")} {s.err.toFixed(settings.precision)}</div>
									<div>rank {s.rank}</div>
								</button>
							</div>
						)}
					</div>
				</div>
			}
		</section>
		<div className="container px-4 flex-grow-1">
			{store.solutions.map((s, i) =>
				<SolutionComponent key={i} data={s} show={sol == i} onSelect={() => setSol(i)} />
			)}
		</div>
		<Footer />
	</>;
}

export default App;

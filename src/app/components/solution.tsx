import { useTranslation } from "react-i18next";
import { Solution } from "../store";
import { StepComponent } from "./step";
import { Diagram } from "./diagram";

interface SolutionComponentProps {
	data: Solution;
	show: boolean;
	onSelect: () => void;
}

export function SolutionComponent({ data, show, onSelect }: SolutionComponentProps) {
	const { t } = useTranslation();
	return (
		<div className={"card mt-3 " + (show ? "" : "d-md-none")} style={{ overflow: "hidden" }}>
			<div className="card-header capitalize d-none d-md-block">
				<span className="d-inline-block">{t("phrase.solution")} {data.solution},</span> <span className="d-inline-block">{t("phrase.error")} {data.err},</span> <span className="d-inline-block">rank {data.rank}</span>
			</div>
			<div className={"card-header capitalize d-md-none " + (show ? "text-bg-primary" : "")} onClick={onSelect} style={{ cursor: "pointer" }}>
				<span className="d-inline-block">{t("phrase.solution")} {data.solution},</span> <span className="d-inline-block">{t("phrase.error")} {data.err},</span> <span className="d-inline-block">rank {data.rank}</span>
			</div>
			<div className={"card-body " + (show ? "" : "d-none")}>
				{data.steps.length == 0 && (
					<div className="row justify-content-center">
						<div className="col" style={{ flex: "0 1 12rem" }}>
							<Diagram data={data.diagrams[0]} last={true} />
						</div>
						<div className="col p-2" style={{ flex: "1 1 12rem" }}></div>
					</div>
				)}
				{data.steps.map((step, i) => (
					<div className="row justify-content-center" key={i}>
						<div className="col" style={{ flex: "0 1 12rem" }}>
							<Diagram data={data.diagrams[i]} last={i == data.steps.length - 1} />
						</div>
						<div className="col p-2" style={{ flex: "1 1 12rem" }}>
							<ol start={i + 1}>
								<StepComponent data={step} last={i == data.steps.length - 1} />
							</ol>
						</div>
					</div>
				))}
			</div>
		</div>
	);
}

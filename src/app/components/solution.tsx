import { useTranslation } from "react-i18next";
import { Solution } from "../store";
import { StepComponent } from "./step";
import { Diagram } from "./diagram";

interface SolutionComponentProps {
	data: Solution;
}

export function SolutionComponent({ data }: SolutionComponentProps) {
	const { t } = useTranslation();
	return (
		<div className="card mt-3">
			<div className="card-header capitalize">
				{t("phrase.solution")} {data.solution}, {t("phrase.error")} {data.err}, rank {data.rank}
			</div>
			<div className="card-body">
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

import { useTranslation } from "react-i18next";
import { Solution } from "../store";
import { StepComponent } from "./step";
import { Diagram } from "./svg/diagram";
import { useRef } from "react";

interface SolutionComponentProps {
	data: Solution;
	show: boolean;
	onSelect: () => void;
}

function getDegree(p: IPoint): string {
	let result = Math.atan2(-p[0], p[1]) / Math.PI * 180;
	if(result <= -90) result += 180;
	if(result > 90) result -= 180;
	return result.toFixed(4) + "°";
}

export function formatSolution(data: Solution): string {
	const item = data.solution[1];
	const text = typeof item == "number" ? item.toFixed(4) : getDegree(item);
	return `(${data.solution[0].toFixed(4)}, ${text})`;
}

export function SolutionComponent({ data, show, onSelect }: SolutionComponentProps) {
	const { t } = useTranslation();
	const ref = useRef<HTMLDivElement>(null);
	const handleSelect = () => {
		onSelect();
		setTimeout(() => ref.current?.scrollIntoView(), 0);
	}
	const solution = formatSolution(data);

	const err = data.err.toFixed(4);

	return (
		<div className={"card mt-3 " + (show ? "" : "d-sm-none")} style={{ overflow: "hidden" }}>
			<div className="card-header d-none d-sm-block">
				<span className="d-inline-block capitalize">{t("phrase.solution")} {solution},</span> <span className="d-inline-block">{t("phrase.error")} {err},</span> <span className="d-inline-block">rank {data.rank}</span>
			</div>
			{show ? (
				<div ref={ref} className="card-header d-sm-none text-bg-primary">
					<span className="d-inline-block capitalize">{t("phrase.solution")} {solution},</span> <span className="d-inline-block">{t("phrase.error")} {err},</span> <span className="d-inline-block">rank {data.rank}</span>
				</div>
			) : (
				<div ref={ref} className="card-header d-sm-none" onClick={handleSelect} style={{ cursor: "pointer" }}>
					<div className="row gx-0 justify-content-center align-items-top">
						<div className="col solution-preview" style={{ flex: "0 1 9rem" }}>
							<Diagram data={data.diagrams[data.diagrams.length - 1]} />
						</div>
						<div className="col mb-2 ps-3 mt-2" style={{ flex: "1 0 8rem" }}>
							<div>{solution}</div>
							<div>{t("phrase.error")} {err}</div>
							<div>rank {data.rank}</div>
						</div>
					</div>
				</div>
			)}

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

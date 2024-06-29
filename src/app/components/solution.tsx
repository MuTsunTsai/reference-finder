import { useTranslation } from "react-i18next";
import { Solution, useSettings } from "../store";
import { StepComponent } from "./step";
import { DIAGRAM_ZOOM, Diagram } from "./svg/diagram";
import { useRef } from "react";

interface SolutionComponentProps {
	data: Solution;
	show: boolean;
	onSelect: () => void;
}

const DIAGRAM_PADDING = 0.18;
const DIAGRAM_SIZE = `${12 * DIAGRAM_ZOOM}rem`;

/**
 * Calculates the degree using the normal vector of a line.
 */
function getDegree(p: IPoint, precision: number): string {
	let result = Math.atan2(-p[0], p[1]) / Math.PI * 180;
	if(result <= -90) result += 180;
	if(result > 90) result -= 180;
	return result.toFixed(precision) + "°";
}

/**
 * Original ReferenceFinder represents a line as (distance to origin, normal unit vector),
 * and I think using (distance to origin, degree of the line) is more intuitive.
 */
export function formatSolution(data: Solution, precision: number): string {
	const item = data.solution[1];
	const text = typeof item == "number" ? item.toFixed(precision) : getDegree(item, precision);
	return `(${data.solution[0].toFixed(precision)}, ${text})`;
}

export function SolutionComponent({ data, show, onSelect }: SolutionComponentProps) {
	const { t } = useTranslation();
	const settings = useSettings();
	const ref = useRef<HTMLDivElement>(null);
	const handleSelect = () => {
		onSelect();
		setTimeout(() => ref.current?.scrollIntoView(), 0);
	};
	const solution = formatSolution(data, settings.precision);

	const err = data.err.toFixed(settings.precision);

	return (
		<div className={"card mt-3 " + (show ? "" : "d-sm-none")} style={{ overflow: "hidden" }}>
			<div className="card-header d-none d-sm-block">
				<span className="d-inline-block capitalize">{t("phrase.solution")} {solution},</span> <span className="d-inline-block">{t("phrase.error")} {err},</span> <span className="d-inline-block">rank {data.rank}</span>
			</div>
			{show ?
				<div ref={ref} className="card-header d-sm-none text-bg-primary">
					<span className="d-inline-block capitalize">{t("phrase.solution")} {solution},</span> <span className="d-inline-block">{t("phrase.error")} {err},</span> <span className="d-inline-block">rank {data.rank}</span>
				</div> :
				<div ref={ref} className="card-header d-sm-none" onClick={handleSelect} style={{ cursor: "pointer" }}>
					<div className="row gx-0 justify-content-center align-items-top">
						<div className="col solution-preview" style={{ flex: "0 1 9rem" }}>
							<Diagram data={data.diagrams[data.diagrams.length - 1]} padding={DIAGRAM_PADDING} />
						</div>
						<div className="col mb-2 ps-3 mt-2" style={{ flex: "1 0 8rem" }}>
							<div>{solution}</div>
							<div>{t("phrase.error")} {err}</div>
							<div>rank {data.rank}</div>
						</div>
					</div>
				</div>
			}

			<div className={"card-body " + (show ? "" : "d-none")}>
				{data.steps.length == 0 &&
					<div className="row justify-content-center">
						<div className="col" style={{ flex: `0 1 ${DIAGRAM_SIZE}` }}>
							<Diagram data={data.diagrams[0]} last={true} padding={DIAGRAM_PADDING} />
						</div>
						<div className="col p-2" style={{ flex: `1 1 ${DIAGRAM_SIZE}` }}></div>
					</div>
				}
				<div className="row">
					{data.steps.map((step, i) =>
						<div className="col-12 col-md-4 mb-sm-3" key={i}>
							<div className="row justify-content-center">
								<div className="col" style={{ flex: `0 1 ${DIAGRAM_SIZE}` }}>
									<Diagram
										data={data.diagrams[i]}
										last={i == data.steps.length - 1}
										padding={DIAGRAM_PADDING}
									/>
								</div>
								<div className="col p-2" style={{ flex: `1 1 ${DIAGRAM_SIZE}` }}>
									<ol start={i + 1}>
										<StepComponent data={step} last={i == data.steps.length - 1} />
									</ol>
								</div>
							</div>
						</div>
					)}
				</div>
			</div>
		</div>
	);
}

import { useTranslation } from "react-i18next";
import { ElementType, LineStyle, PointStyle, Solution, useDB, useSettings, useStore } from "../store";
import { StepComponent } from "./step";
import { DIAGRAM_ZOOM, Diagram } from "./svg/diagram";
import { useRef } from "react";
import { Exact } from "./exact";
import { resetWorker } from "../bridge";

interface SolutionComponentProps {
	data: Solution;
	show: boolean;
	onSelect: () => void;
}

interface AddButtonProps {
	data: Solution;
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

/**
 * Function that runs when the user accepts a specific solution.
 * Adds the creases from the last diagram of the accepted solution and recreates the database.
 */
function addAsExistingCreases(data: Solution) {
	gtag("event", "ref_add_existing_refs");

	// Add the creases from the last diagram of the accepted solution to the store (to render on the preview paper)
	const { existingMarks, existingLines, existingRefs } = useStore.getState();
	for(let i = 0; i < data.steps.length; i++) {
		let step = data.steps[i];
		const diag = data.diagrams[i];
		if(step.axiom != 0) { // line
			const lookFor = step.pinch ? LineStyle.pinch : LineStyle.valley;
			for(const el of diag) {
				if(el && el.type == ElementType.line && el.style == lookFor) {
					el.style = LineStyle.crease;
					existingRefs.push(el);
					if(!step.pinch) existingLines.push([el.from, el.to]);
					break;
				}
			}
			if(step.intersection) step = step.intersection; // continue with the next part
		}
		if(step.axiom == 0) { // point
			for(const el of diag) {
				if(el && el.type == ElementType.point && el.style == PointStyle.action) {
					el.style = PointStyle.normal;
					existingRefs.push(el);
					existingMarks.push(el.pt);
					break;
				}
			}
		}
	}
	useStore.setState({
		existingRefs, existingMarks, existingLines, // Add the existing marks and lines to the store
		solutions: [], // Empty out the list of solutions so the user notices that the input mark/line is solved
	});

	// Recreate the database
	resetWorker(useDB.getState());
}

function AddButton({ data }: AddButtonProps) {
	const { t } = useTranslation();
	return (
		<button className="btn btn-success py-0" onClick={() => addAsExistingCreases(data)}>
			<i className="fa-solid fa-circle-plus"></i>
			<span className="d-inline-block capitalize m-1">&nbsp;{t("phrase.addAsExistingRefs")}</span>
		</button>
	);
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
				<div className="row align-items-center gx-1 gy-2">
					<div className="col-auto">
						<Exact err={data.err} />&nbsp;<span className="capitalize">{t("phrase.solution")} {solution},</span>
					</div>
					<div className="col-auto">{t("phrase.error")} {err},</div>
					<div className="col-auto">rank {data.rank}</div>
					<div className="col d-none d-md-block text-end" style={{ flex: "1 0 max-content" }}>
						<AddButton data={data} />
					</div>
				</div>
			</div>
			{show ?
				<div ref={ref} className="card-header d-sm-none text-bg-primary">
					<div className="row align-items-center gx-1">
						<div className="col-auto">
							<Exact err={data.err} />&nbsp;<span className="capitalize">{t("phrase.solution")} {solution},</span>
						</div>
						<div className="col-auto">{t("phrase.error")} {err},</div>
						<div className="col-auto">rank {data.rank}</div>
					</div>
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
				<div className="text-end d-md-none mt-2">
					<AddButton data={data} />
				</div>
			</div>
		</div>
	);
}

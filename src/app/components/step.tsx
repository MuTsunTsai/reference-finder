import { useTranslation } from "react-i18next";
import { Step, StepBase, useSettings } from "../store";

interface StepComponentProps {
	data: Step;
	last: boolean;
}

const CJK = /([\u3040-\u30FF\u4E00-\u9FFF\u3400-\u4DBF\uF900-\uFAFF。]) ([\u3040-\u30FF\u4E00-\u9FFF\u3400-\u4DBF\uF900-\uFAFF])/g;

export function StepComponent({ data, last }: StepComponentProps) {
	const { t } = useTranslation();
	const settings = useSettings();
	const step = structuredClone(data);

	function formatStep(s: StepBase) {
		s.p0 = format(s.p0, true);
		s.p1 = format(s.p1, true);
		s.l0 = format(s.l0);
		s.l1 = format(s.l1);
	}

	function format(v: string | undefined, point?: boolean): string | undefined {
		if(typeof v !== "string") return v;
		if(v == v.toLowerCase()) {
			v = t("paper." + v);
		} else if(point) {
			v = t("phrase.point") + " " + v;
		} else {
			v = t("phrase.line") + " " + v;
		}
		return v;
	}

	function purge(v: string): string {
		while(v.match(CJK)) v = v.replace(CJK, "$1$2");
		return v;
	}

	formatStep(step);
	if(step.intersection) formatStep(step.intersection);

	return (
		<li>
			{step.axiom == 0 ?
				purge(t(`intersection_solution`, { ...step })) :
				<>
					<div>
						{settings.showAxiom && <span>[O{step.axiom}] </span>}
						{purge((last && !step.intersection ?
							t(`axiom_solution.${step.axiom - 1}`, { ...step }) :
							t(`axiom.${step.axiom - 1}`, { ...step })) +
							(!step.intersection && step.pinch ? " " + t("pinch") : "")
						)}
						<span> </span>
					</div>
					{step.intersection &&
					<div>
						{purge((last ?
							t(`intersection_solution`, { ...step.intersection }) :
							t(`intersection`, { ...step.intersection })) +
								(step.pinch ? " " + t("pinch") : "")
						)}
					</div>
					}
				</>
			}
		</li>
	);
}

import { useTranslation } from "react-i18next";
import { useContext } from "react";
import { DndContext, DragEndEvent } from "@dnd-kit/core";
import { SortableContext, arrayMove, useSortable, verticalListSortingStrategy } from "@dnd-kit/sortable";
import { restrictToVerticalAxis, restrictToParentElement } from "@dnd-kit/modifiers";
import { CSS } from "@dnd-kit/utilities";

import { InfoTooltip } from "../tooltip";
import { Checkbox } from "../form/checkbox";
import { SettingsContext } from "./context";

interface ItemProps {
	axiom: number;
	onInput: (v: boolean) => void;
}

// DndKit is pretty cool, but it doesn't support Safari 12.
// In that case we fallback to use up-down buttons.
// We use this signature to detect legacy browsers.
const legacyBrowser = typeof Intl.PluralRules == "undefined";

const Item = ({ axiom, onInput }: ItemProps) => {
	const { t } = useTranslation();
	const { tempDb } = useContext(SettingsContext);
	const { setNodeRef, transform, transition, attributes, listeners } = useSortable({ id: axiom });
	const style = {
		transform: CSS.Transform.toString(transform),
		transition,
	};

	return <div ref={setNodeRef} style={style} className="d-flex align-items-baseline">
		<div className="me-3 handle" {...attributes} {...listeners}>
			<i className="fa-solid fa-grip-lines"></i>
		</div>
		<Checkbox value={tempDb.axioms[axiom - 1]} onInput={onInput}>
			O{axiom} - {t(`settings.basic.axioms.O${axiom}`)}
		</Checkbox>
	</div>;
};

export function Axioms() {
	const { t } = useTranslation();
	const { tempDb, setTempDb } = useContext(SettingsContext);

	function handleDragEnd(event: DragEndEvent) {
		const { active, over } = event;
		if(over && active.id !== over.id) {
			const oldIndex = tempDb.axiomPriority.indexOf(active.id as number);
			const newIndex = tempDb.axiomPriority.indexOf(over.id as number);
			move(oldIndex, newIndex);
		}
	}

	function move(oldIndex: number, newIndex: number) {
		setTempDb({ ...tempDb, axiomPriority: arrayMove(tempDb.axiomPriority, oldIndex, newIndex) });
	}

	const setAxiom = (index: number, v: boolean) => {
		const axioms = tempDb.axioms.concat();
		axioms[index] = v;
		setTempDb({ ...tempDb, axioms });
	};

	return (
		<div className="mb-1">
			<h6>{t("settings.basic.axioms._")}<InfoTooltip title={t("help.axioms")} /></h6>
			{legacyBrowser ?
				<div>
					{tempDb.axiomPriority.map((a, i) =>
						<div className="d-flex align-items-baseline" key={a}>
							<div className="me-3" >
								<button type="button" className="btn btn-ld btn-sm py-0 px-1 me-2"
									disabled={i == 0} onClick={() => move(i, i - 1)}>
									<i className="fa-solid fa-caret-up"></i>
								</button>
								<button type="button" className="btn btn-ld btn-sm py-0 px-1"
									disabled={i == 6} onClick={() => move(i, i + 1)}>
									<i className="fa-solid fa-caret-down"></i>
								</button>
							</div>
							<Checkbox value={tempDb.axioms[a - 1]} onInput={v => setAxiom(a - 1, v)}>
								O{a} - {t(`settings.basic.axioms.O${a}`)}
							</Checkbox>
						</div>
					)}
				</div> :
				<div>
					<DndContext onDragEnd={handleDragEnd}
						modifiers={[restrictToVerticalAxis, restrictToParentElement]}>
						<SortableContext items={tempDb.axiomPriority} strategy={verticalListSortingStrategy}>
							{tempDb.axiomPriority.map((a, i) =>
								<Item key={a} axiom={a} onInput={v => setAxiom(a - 1, v)} />
							)}
						</SortableContext>
					</DndContext>
				</div>
			}
		</div>
	);
}

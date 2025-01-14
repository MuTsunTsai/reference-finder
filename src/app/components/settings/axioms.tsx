import { useTranslation } from "react-i18next";
import { useContext } from "react";
import {
	DndContext,
	useSensor, useSensors, MouseSensor, TouchSensor, // Needed to work properly on mobile devices
	closestCenter // This helps prevent weird behavior near the boundary of the parent container
} from "@dnd-kit/core";
import { SortableContext, arrayMove, useSortable } from "@dnd-kit/sortable";
import { CSS } from "@dnd-kit/utilities";

import { InfoTooltip } from "../tooltip";
import { Checkbox } from "../form/checkbox";
import { SettingsContext } from "./context";

import type { DragEndEvent } from "@dnd-kit/core";

interface AxiomProps {
	axiom: number;
}

// DndKit is pretty cool, but it doesn't support Safari 12.
// In that case we fallback to use up-down buttons.
// We use this signature to detect legacy browsers.
const legacyBrowser = typeof Intl.PluralRules == "undefined";

const Axiom = ({ axiom }: AxiomProps) => {
	const { t } = useTranslation();
	const { tempDb, setTempDb } = useContext(SettingsContext);

	const setAxiom = (v: boolean) => {
		const axioms = tempDb.axioms.concat();
		axioms[axiom - 1] = v;
		setTempDb({ ...tempDb, axioms });
	};

	return <Checkbox value={tempDb.axioms[axiom - 1]} onInput={setAxiom}>
		O{axiom} - {t(`settings.basic.axioms.O${axiom}`)}
	</Checkbox>;
};

const Item = (props: AxiomProps) => {
	const { setNodeRef, transform, transition, attributes, listeners } =
		useSortable({ id: props.axiom });
	const style = {
		transform: CSS.Transform.toString(transform),
		transition,
	};

	return <div ref={setNodeRef} style={style} className="d-flex align-items-top">
		<div className="me-3 handle" {...attributes} {...listeners}>
			<i className="fa-solid fa-grip-lines"></i>
		</div>
		<Axiom {...props} />
	</div>;
};

export function Axioms() {
	const { t } = useTranslation();
	const { tempDb, setTempDb } = useContext(SettingsContext);
	const sensors = useSensors(
		useSensor(MouseSensor, {
			activationConstraint: {
				distance: 5,
			},
		}),
		useSensor(TouchSensor, {
			activationConstraint: {
				delay: 50,
				tolerance: 5,
			},
		})
	);

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

	return (
		<div className="mb-1">
			<h6>{t("settings.basic.axioms._")}<InfoTooltip title={t("help.axioms")} /></h6>
			{legacyBrowser ?
				<div>
					{tempDb.axiomPriority.map((a, i) =>
						<div className="d-flex align-items-top" key={a}>
							<div className="me-3">
								<button
									type="button"
									className="btn btn-ld btn-sm py-0 px-1 me-2"
									disabled={i == 0}
									onClick={() => move(i, i - 1)}
								>
									<i className="fa-solid fa-caret-up"></i>
								</button>
								<button
									type="button"
									className="btn btn-ld btn-sm py-0 px-1"
									disabled={i == 6}
									onClick={() => move(i, i + 1)}
								>
									<i className="fa-solid fa-caret-down"></i>
								</button>
							</div>
							<Axiom axiom={a} />
						</div>
					)}
				</div> :
				<div>
					<DndContext onDragEnd={handleDragEnd} sensors={sensors} collisionDetection={closestCenter}>
						<SortableContext items={tempDb.axiomPriority}>
							{tempDb.axiomPriority.map((a, i) =>
								<Item key={a} axiom={a} />
							)}
						</SortableContext>
					</DndContext>
				</div>}
		</div>
	);
}

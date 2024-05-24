import { createContext } from "react";

import type { Dispatch, SetStateAction } from "react";
import type { DbSettings } from "../../store";

export const SettingsContext = createContext<{
	tempDb: DbSettings;
	setTempDb: Dispatch<SetStateAction<DbSettings>>;
}>(undefined!);

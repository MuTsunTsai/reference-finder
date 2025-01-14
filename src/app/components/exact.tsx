import { useTranslation } from "react-i18next";

interface ExactProps {
	err: number;
}

const EPS = 1e-15;

export function Exact({ err }: ExactProps) {
	const { t } = useTranslation();
	return (err < EPS ? <span title={t("tip.exact")}>⭐</span> : <></>);
}

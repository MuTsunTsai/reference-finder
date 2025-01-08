import { useTranslation } from "react-i18next";

interface ExactProps {
	err: number;
}

export function Exact({ err }: ExactProps) {
	const { t } = useTranslation();
	return (err < 1e-15 ? <span title={t("tip.exact")}>⭐</span> : <></>);
}

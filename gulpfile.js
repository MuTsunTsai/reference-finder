import gulp from "gulp";
import fontAwesome from "gulp-fontawesome";
import through2 from "gulp-through2";

export default () =>
	gulp.src("src/app/**/*.tsx")
		.pipe(fontAwesome())
		.pipe(gulp.dest("src/icon"));

function pad(n) {
	return (n < 10 ? "0" : "") + n;
}

export const syncVer = async () => {
	const pkg = await import("./package.json", { with: { type: "json" } });
	const match = pkg.default.version.match(/\d+/g);
	return gulp.src("src/core/RFVersion.h")
		.pipe(through2(content => {
			const newContent = content
				.replace(/(?<=VERSION_MAJOR )\d+/, match[0])
				.replace(/(?<=VERSION_MINOR )\d+/, match[1])
				.replace(/(?<=VERSION_BUGFIX )\d+/, match[2]);
			if(newContent === content) return null;

			const now = new Date();
			const today = now.getFullYear() + pad(now.getMonth() + 1) + pad(now.getDate());
			return newContent.replace(/(?<=VERSION_BUILD )\d+/, today);
		}))
		.pipe(gulp.dest("src/core"));
};

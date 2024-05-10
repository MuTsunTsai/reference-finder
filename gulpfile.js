const gulp = require("gulp");
const fontAwesome = require("gulp-fontawesome");
const through2 = require("gulp-through2");

gulp.task("default", () =>
	gulp.src("src/app/**/*.tsx")
		.pipe(fontAwesome())
		.pipe(gulp.dest("src/lib"))
);

function pad(n) {
	return (n < 10 ? "0" : "") + n;
}

gulp.task("syncVer", async () => {
	const pkg = await require("./package.json");
	const match = pkg.version.match(/\d+/g);
	const now = new Date();
	const today = now.getFullYear() + pad(now.getMonth() + 1) + pad(now.getDate());
	return gulp.src("src/core/RFVersion.h")
		.pipe(through2(content => content
			.replace(/(?<=VERSION_MAJOR )\d+/, match[0])
			.replace(/(?<=VERSION_MINOR )\d+/, match[1])
			.replace(/(?<=VERSION_BUGFIX )\d+/, match[2])
			.replace(/(?<=VERSION_BUILD )\d+/, today)
		))
		.pipe(gulp.dest("src/core"));
});

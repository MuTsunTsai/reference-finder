const gulp = require("gulp");
const fontAwesome = require("gulp-fontawesome");

gulp.task("default", () =>
	gulp.src("src/app/**/*.tsx")
		.pipe(fontAwesome())
		.pipe(gulp.dest("src/lib"))
);

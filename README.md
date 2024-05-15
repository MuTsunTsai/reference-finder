
# ReferenceFinder

> Find reference points and lines in crease patterns.

ReferenceFinder was originally developed by [Robert J. Lang](https://langorigami.com/article/referencefinder/),
and an elegant web version of it was developed by [Maya Kraft](https://github.com/mayakraft/ReferenceFinder) (f.k.a. Robby Kraft) using a Node.js backend querying a pre-generated database.
Unfortunately, the latter no longer works as Heroku discontinues its free tier service in November 2022.

The current app recreates the ReferenceFinder for web by compiling the original [C++ source code](https://github.com/bugfolder/ReferenceFinder) to WebAssembly and running it purely within the browser.
This allows the app to be deployed on GitHub pages (ensuring longevity) and enables all original features of the original app,
such as changing the sheet dimensions and customize settings for database generating.

## For contributors

If you would like to contribute, checkout our
[Contributing Guidelines](CONTRIBUTING.md) for more details.

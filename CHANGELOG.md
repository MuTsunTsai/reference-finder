
# Change log

## v4.7

- Add lightbox to the preview and diagram images.

#### Patches

- v4.7.1: Fix scoring system.
- v4.7.2: Improve memory efficiency.

## v4.6

- One can add all references in a solution as existing references, and continue the search from there.
- Fixes descriptions in axioms 5, 6, and 7.

## v4.5

- Allowing disabling divisions to generate all references found (up to rank 5).
- Add (probably) exact folding sequence indicator.

#### Patches

- v4.5.1: Improve memory efficiency and performance.

## v4.4

- Caching the generated database to the device, allowing much faster startup afterwards.

#### Patches
- v4.4.1: Improve memory efficiency, and use greatly speed up statistics runtime.
- v4.4.2: Further improve memory efficiency.
- v4.4.3: Add random seed to the statistics.
- v4.4.4: Codebase refactoring.
- v4.4.5: Filter out essentially identical solutions (fixes issue [#1](https://github.com/MuTsunTsai/reference-finder/issues/1)).

## v4.3

- Fixed a bug in the original ReferenceFinder that could result in overlooked solutions for points on the boundary of the sheet.
- Introducing the scoring system that replaces references that were already found with better equivalences found later. This will make the initialization slightly slower, but will generate much better folding sequences that honors the priority settings.

## v4.2

- Allowing customizing the priorities of the 7 axioms.

## v4.1

- If a line is later only used to make one intersection, we render it as a pinch instead of a whole line.

#### Patches
- v4.1.1: Fix bug in label rendering.
- v4.1.2: Reduce the likelihood of drawing very small arrows.
- v4.1.3: Fix several minor bugs.
- v4.1.4: Refactoring and improve initialization performance.
- v4.1.5: Making sure that original references are place at the beginning of the sequence.

## v4.0.3

Version 4.0.3 made some significant initialization performance improvements (about 40% faster) by means of the following:

- Using `std::unordered_map` instead of `std::map`.
- Used only one map (instead of per rank) to check for duplications, and then use vectors for the per rank map and the buffer.

## v4.0.2

- Fixed a bug that results in v4.0.1 overlooking some solutions related to axiom O5 (See also [PR#1](https://github.com/bugfolder/ReferenceFinder/pull/1)).
- Cleaned up unused code.
- Fully migrated to use `JsonObject` and `JsonArray` classes to generate output.

## v4.0.1

- The starting point of ReferenceFinder for web.
- For the most part, it is the same as Lang's original source code (with some refactoring).
	I only did some minor modifications to make it interop with JavaScript.Most notably, I have a new `JsonStreamDgmr` class replacing the original `VerbalStreamDgmr`.

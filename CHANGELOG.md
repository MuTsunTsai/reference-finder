
# Change log

## v4.1.1

Fix bug in label rendering.

## v4.1.0

If a line is later only used to make one intersection, we render it as a pinch instead of a whole line.

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

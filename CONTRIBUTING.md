# Contributing Guidelines

It is recommended that you use [VS Code](https://code.visualstudio.com/) for this project.

## Environment and build instructions

To build and run this project, you will need the following:
- [Node.js](https://nodejs.org/)
- [PNPM](https://pnpm.io/), and then execute the following in the project folder: `pnpm install`.
- [Emscripten](https://emscripten.org/), and make sure that `emcc` is available in PATH.\
	You may also need to change `configurations.compilerPath` in the
	[.vscode/c_cpp_properties.json](./.vscode/c_cpp_properties.json)
	file to reflect the location of your Emscripten installation.
- [GNU make](https://community.chocolatey.org/packages/make), if you're on Windows.\
	(Our makefile should work on all desktop platforms regardlessly.)

Then you can run the project by the following:

```bash
pnpm start
```

This will build the project and start the development server.

## Project structure

All source codes are located in the `src` folder, in which you'll find:

- `app`: User interface, written in [React](https://react.dev/) (I usually use Vue, but I decided to give it a try for this project).
- `core`: The C++ part. See [change log](./CHANGELOG.md) for the changes made to the original source code of ReferenceFinder.
- `icon`: This folder contains the pre-built Font Awesome subset.\
  If you add more icons and need to rebuild this folder, execute `pnpm gulp`.
- `lib`: Compiled WebAssembly of ReferenceFinder.
- `locale`: All locale files, based on the previous work by [Maya Kraft](https://github.com/mayakraft/ReferenceFinder) etc.
- `public`: The HTML file and other unbundled assets.

## How to contribute new ideas/features

Always talk to me first! Just to avoid wasting time working on something that I'm also working on. A good place to reach me will be the [BP Studio Discord server](https://discord.gg/HkcdTDS4zZ).


### Submitting pull requests

Because merging to the main branch will directly publish the changes,
do not directly submit a PR against the main branch.
Instead, contact me and I will create a new branch for you to submit your PR.
After I merge the PR and make sure that everything is ready,
it will be merged into the main branch.

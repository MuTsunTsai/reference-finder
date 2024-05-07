# Contributing Guidelines

It is recommended that you use [VS Code](https://code.visualstudio.com/) for this project.

## Environment and build instructions

To build and run this project, you will need the following:
- [Node.js](https://nodejs.org/)
- [PNPM](https://pnpm.io/), and then execute the following in the project folder: `pnpm install`.
- [Emscripten](https://emscripten.org/), and make sure that `emcc` is available in PATH.\
	You may also need to change `configurations.includePath` in the [.vscode/c_cpp_properties.json](./.vscode/c_cpp_properties.json) file to reflect the location of your Emscripten installation.
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
- `lib`: This folder contains the compiled WebAssembly and some bridging scripts. It also contains the pre-built Font Awesome subset.
- `locale`: All locale files, based on the previous work by [Robby Kraft](https://github.com/robbykraft/ReferenceFinder) etc.
- `public`: The HTML file and other unbundled assets.

# Contributing Guidelines

It is recommended that you use [VS Code](https://code.visualstudio.com/) for this project.

## Environment and build instructions

To build and run this project, you will need the following:
- [Node.js](https://nodejs.org/)
- [PNPM](https://pnpm.io/), and then execute the following in the project folder: `pnpm install`.
- [Emscripten](https://emscripten.org/), and make sure that `emcc` is available in PATH.
- [GNU make](https://community.chocolatey.org/packages/make), if you're on Windows.

Then you can run the project by the following:

```bash
pnpm start
```

This will build the project and start the development server.

## Project structure

All source codes are located in the `src` folder, in which you'll find:

- `app`: User interface, written in React (I usually use Vue, but I decided to give it a try for this project).
- `core`: The C++ part. See [change log](./CHANGELOG.md) for the changes made to the original source code of ReferenceFinder.
- `lib`: This contains the compiled WebAssembly and some bridging scripts.
- `locale`: All locale files, based on the previous work by Robby Kraft etc.
- `public`: The HTML file and other unbundled assets.

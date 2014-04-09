# Sea Core Explorer

## Development instructions

This project uses VTK 6.1 and Qt 4.8.5. VTK will be downloaded and built via a
CMake "External Project" specified in the `Superbuild` directory. Qt is assumed
to be installed via standard means (I installed using homebrew on OS X). To
build:

```bash
git clone path.to.this.repository
mkdir SeaCoreExplorer-superbuild
cd SeaCoreExplorer-superbuild
cmake ../SeaCoreExplorer
make
```

This will build the project and dependencies. To build the project from now on
`cd SeaCoreExplorer-build` to descend into the inner build directory. Once
there, `make` will only build this project. To run the application simply type
`./bin/SeaCoreExplorer`.

Hurray!

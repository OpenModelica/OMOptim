# OMOptim

OMOptim is an optimization software for Modelica models.

## Dependencies

- [OpenModelica](https://github.com/OpenModelica/OpenModelica)
- g++ (clang++ does not work)

## Build instructions

Follow the instructions matching your OS:

- [OMCompiler/README.Linux.md](https://github.com/OpenModelica/OpenModelica/blob/master/OMCompiler/README.Linux.md)
- [OMCompiler/README.Windows.md](https://github.com/OpenModelica/OpenModelica/blob/master/OMCompiler/README.Windows.md)

On Windows, OMOptim is built as part of the normal CMake `install` target (see
`OM_ENABLE_GUI_CLIENTS` in [README.cmake.md](https://github.com/OpenModelica/OpenModelica/blob/master/README.cmake.md)); no extra step is needed. Start OMOptim from
`/path/to/OpenModelica/build_cmake/install_cmake/bin/OMOptim.exe`.

## Bug Reports

- Submit bugs through the [OpenModelica GitHub issues](https://github.com/OpenModelica/OpenModelica/issues/new).
- [Pull requests](../../pulls) are welcome ❤️

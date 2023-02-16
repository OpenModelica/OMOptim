# OMOptim
OMOptim is an optimization software for Modelica models.

## Dependencies

  - [OpenModelica](https://github.com/OpenModelica/OpenModelica), compiled with CORBA support
  - g++ (clang++ does not work)
  - CORBA implementation (omniORB, or possibly MICO)

## Build instructions

Follow the instructions matching your OS:

  - [OMCompiler/README.Linux.md](https://github.com/OpenModelica/OpenModelica/blob/master/OMCompiler/README.Linux.md)
  - [OMCompiler/README.Windows.md](https://github.com/OpenModelica/OpenModelica/blob/master/OMCompiler/README.Windows.md)

### Windows MSYS Makefiles

If you used MSYS Makefiles to compile OpenModelica you need one additional step:

Start a MSYS terminal `$OMDEV\tools\msys\mingw64.exe` (64 bit) or
`$OMDEV\tools\msys\mingw32.exe` (32 bit) and run:

```bash
$ cd /path/to/OpenModelica
make -f Makefile.omdev.mingw omoptim -j<Nr. of cores>
```

Start OMOptim from `/path/to/OpenModelica/build/bin/OMOptim.exe`

## Bug Reports

  - Submit bugs through the [OpenModelica GitHub issues](https://github.com/OpenModelica/OpenModelica/issues/new).
  - [Pull requests](../../pulls) are welcome ❤️

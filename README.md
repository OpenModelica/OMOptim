# OMOptim
OMOptim is an optimization software for Modelica models.

## Dependencies

- [OpenModelica](https://openmodelica.org), compiled with CORBA support
- ParadisEO (exactly version 1.3-beta2; will be added to the builds eventually)
- g++-4.4 (because the used version of ParadisEO only works with this version of g++)
- CORBA implementation (omniORB, or possibly MICO)

## Build instructions

Install the dependencies.

```bash
$ autoconf
# OPENMODELICAHOME is usually /usr, /opt, /opt/openmodelica, or /path/to/svn/OpenModelica/build
$ ./configure --prefix=/path/to/OPENMODELICAHOME CC=gcc-4.4 CXX=g++-4.4
$ make
$ make install
```

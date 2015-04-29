# OMOptim
OMOptim is an optimization software for Modelica models.

## Dependencies

- [OpenModelica](https://openmodelica.org), compiled with CORBA support
- g++ (clang++ does not work)
- CORBA implementation (omniORB, or possibly MICO)

## Build instructions

Install the dependencies.

```bash
$ autoconf
# OPENMODELICAHOME is usually /usr, /opt, /opt/openmodelica, or /path/to/svn/OpenModelica/build
$ ./configure --prefix=/path/to/OPENMODELICAHOME CXX=g++
$ make
$ make install
```

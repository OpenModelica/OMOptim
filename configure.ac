dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([OMOptim],[dev],[https://trac.openmodelica.org/OpenModelica],[openmodelica],[https://openmodelica.org])

AC_SUBST(IDLCMD)
AC_SUBST(RPATH_QMAKE)

dnl Checks for programs.
AC_LANG([C++])
AC_PROG_CXX

AC_CHECK_PROGS(QMAKE,qmake-qt4 qmake-mac qmake,"")

# AC_CHECK_HEADER(qwt_global.h, [], [AC_MSG_ERROR([Failed to find qwt])])

SAVED_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -L$prefix/lib/omc -lomqwt"
AC_LINK_IFELSE([AC_LANG_PROGRAM([],[])], [], [AC_MSG_ERROR([OpenModelica version of qwt not found.])])
LDFLAGS=$SAVED_LDFLAGS

OMNIORB_DEFAULT=yes
m4_include([corba.m4])

if test -z "$USE_CORBA"; then
  AC_MSG_ERROR([OMOptim requires using the OMC CORBA interface. Configure using --with-omniORB.])
fi

if test "$DARWIN" = "1"; then
  APP=".app"
  EXE=".app"
  SHREXT=".dylib"
  RPATH_QMAKE="-Wl,-rpath,'@loader_path/../../../../lib/omc',-rpath,'@loader_path/../../../../lib/',-rpath,'$PREFIX/lib/omc',-rpath,'$PREFIX/lib/'"
elif test "$host" = "i586-pc-mingw32msvc"; then
  APP=".exe"
  EXE=".exe"
  # Yes, we build static libs on Windows, so the "shared" extension is .a
  SHREXT=".a"
  RPATH_QMAKE="-Wl,-z,origin -Wl,-rpath,\\'\\\$\$ORIGIN/../lib/omc\\' -Wl,-rpath,\\'\\\$\$ORIGIN\\'"
else
  APP=""
  EXE=""
  SHREXT=".so"
  RPATH_QMAKE="-Wl,-z,origin -Wl,-rpath,\\'\\\$\$ORIGIN/../lib/omc\\' -Wl,-rpath,\\'\\\$\$ORIGIN\\'"
fi

AC_OUTPUT(Makefile OMOptim/build/Makefile.unix OMOptim/build/OMOptim.config OMOptimBasis/build/Makefile.unix OMOptimBasis/build/OMOptimBasis.config OMOptim/Core/OMC/omc_config.h)
# AC_CONFIG_COMMANDS([Clean qmake stuff],[make clean])

dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([OMOptim],[dev],[https://trac.openmodelica.org/OpenModelica],[openmodelica],[https://openmodelica.org])

AC_SUBST(APP)
AC_SUBST(EXE)
AC_SUBST(IDLCMD)
AC_SUBST(RPATH_QMAKE)
AC_SUBST(host_short)

m4_include([common/m4/pre-commit.m4])

cp common/install-sh common/config.guess common/config.sub ./

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

rm install-sh

dnl Checks for programs.
AC_LANG([C++])
AC_PROG_CC
AC_PROG_CXX

host_short=$host_cpu-$host_os

m4_include([common/m4/qmake.m4])

m4_include([common/m4/omhome.m4])

SAVED_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -L$prefix/lib/$host_short/omc -lomqwt"
AC_LINK_IFELSE([AC_LANG_PROGRAM([],[])], [], [AC_MSG_ERROR([OpenModelica version of qwt not found.])])
LDFLAGS=$SAVED_LDFLAGS

OMNIORB_DEFAULT=yes
m4_include([common/m4/corba.m4])

if test -z "$USE_CORBA"; then
  AC_MSG_ERROR([OMOptim requires using the OMC CORBA interface. Configure using --with-omniORB.])
fi

AC_OUTPUT(Makefile OMOptim/build/Makefile.unix OMOptim/build/OMOptim.config OMOptimBasis/build/Makefile.unix OMOptimBasis/build/OMOptimBasis.config OMOptim/Core/OMC/omc_config.h)
# AC_CONFIG_COMMANDS([Clean qmake stuff],[make clean])

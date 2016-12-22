dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([OMOptim],[dev],[https://trac.openmodelica.org/OpenModelica],[openmodelica],[https://openmodelica.org])

AC_SUBST(OMOPTIMCXX)
AC_SUBST(APP)
AC_SUBST(EXE)
AC_SUBST(IDLCMD)
AC_SUBST(RPATH_QMAKE)
AC_SUBST(host_short)
AC_SUBST(SOURCE_REVISION)
AC_SUBST(LSB_RELEASE)


m4_include([common/m4/pre-commit.m4])
cp common/install-sh common/config.guess common/config.sub ./
m4_include([common/m4/ombuilddir.m4])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

rm install-sh

dnl Checks for programs.

AC_MSG_CHECKING([if clang++ is used (OMOptim breaks clang++)])
if test ! -z "$GNUCXX"; then
  AC_MSG_RESULT([found explicit GNUCXX; using that])
  CXX="$GNUCXX"
elif echo "$CXX" | grep -q "clang++"; then
  AC_MSG_RESULT([yes; resetting CXX to default autoconf compiler (sorry). Use ./configure GNUCXX=g++ to use clang++ for some projects and g++ only for paradiseo])
  unset CXX
else
  AC_MSG_RESULT([no])
fi

AC_LANG([C++])
AC_PROG_CXX
AC_PROG_CC

host_short=$host_cpu-$host_os

m4_include([common/m4/qmake.m4])
m4_include([common/m4/omhome.m4])

FIND_OPENMODELICAHOME()

FIND_LIBOMQWT()

OMNIORB_DEFAULT=yes
m4_include([common/m4/corba.m4])

AC_MSG_CHECKING([for LSB description])
LSB_RELEASE="`lsb_release -ds | tr -d '\"'`"
if test ! -z "$LSB_RELEASE"; then
  AC_MSG_RESULT([$LSB_RELEASE])
  LSB_RELEASE="#define LSB_RELEASE \"$LSB_RELEASE\""
else
  AC_MSG_RESULT([no])
fi

m4_include([common/m4/semver.m4])
SOURCE_REVISION="$SOURCE_REVISION$NON_FREE_VERSION"

if test -z "$USE_CORBA"; then
  AC_MSG_ERROR([OMOptim requires using the OMC CORBA interface. Configure using --with-omniORB.])
fi

AC_OUTPUT(Makefile OMOptim/build/Makefile.unix OMOptim/build/OMOptim.config OMOptimBasis/build/Makefile.unix OMOptimBasis/build/OMOptimBasis.config OMOptim/omc_config.h)
# AC_CONFIG_COMMANDS([Clean qmake stuff],[make clean])

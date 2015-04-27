dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([OMOptim],[dev],[https://trac.openmodelica.org/OpenModelica],[openmodelica],[https://openmodelica.org])

AC_SUBST(with_paradiseo_include)
AC_SUBST(with_paradiseo_lib)
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

AC_MSG_CHECKING([for paradisEO])
AC_ARG_WITH(paradiseo, [  --with-paradiseo                 Required for OMOptim. Default searches {PREFIX,/usr,/opt} for {include,lib}/paradiseo], [], [with_paradiseo=yes])

if test -f "$with_paradiseo/paradiseo-eo/src/eo"; then
  # User used a local paradiseo installation
  with_paradiseo_include="$with_paradiseo/paradiseo-eo/src $with_paradiseo/paradiseo-mo/src $with_paradiseo/paradiseo-moeo/src $with_paradiseo/paradiseo-eo/src/utils $with_paradiseo/paradiseo-mo/src/utils $with_paradiseo/paradiseo-moeo/src/utils"
  if test -d "$with_paradiseo/paradiseo-eo/build/lib"; then
    paradiseo_build="build"
  else
    paradiseo_build=""
  fi
  with_paradiseo_lib="-L$with_paradiseo/paradiseo-eo/$paradiseo_build/lib -L$with_paradiseo/paradiseo-moeo/$paradiseo_build/lib"
  AC_MSG_RESULT(stock version in: $with_paradiseo)
else
  if test x"$with_paradiseo" = "xyes"; then
    with_paradiseo_checked=""
    for f in /{usr,opt,$PREFIX}; do
      if test -d "$f/include/paradiseo/" -a -d "$f"/lib/paradiseo/; then
        with_paradiseo_include="$f/include/paradiseo/ $f/include/paradiseo/utils/"
        with_paradiseo_lib="-L$f"/lib/paradiseo/
        with_paradiseo="$f"
      elif test -d "$f/include/paradiseo/" -a -f "$f"/lib*/libeo.a ; then
        with_paradiseo_include=`for inc in $f/include/paradiseo/*; do echo -n " $inc"; done`
        with_paradiseo_lib="-L"`dirname "$f"/lib*/libeo.a`
        with_paradiseo="$f"
      else
        with_paradiseo_checked="$with_paradiseo_checked $f"
      fi
    done
  fi

  if test x"$with_paradiseo" = "xno"; then
    with_paradiseo=""
    AC_MSG_RESULT([disabled])
  elif test x"$with_paradiseo" = "xyes"; then
    AC_MSG_ERROR([not found (searched $with_paradiseo_checked)])
  else
    AC_MSG_RESULT($with_paradiseo)
  fi

fi

AC_OUTPUT(Makefile OMOptim/build/Makefile.unix OMOptim/build/OMOptim.config OMOptimBasis/build/Makefile.unix OMOptimBasis/build/OMOptimBasis.config OMOptim/Core/OMC/omc_config.h)
# AC_CONFIG_COMMANDS([Clean qmake stuff],[make clean])

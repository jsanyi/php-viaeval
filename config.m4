dnl $Id$
dnl config.m4 for extension viaeval


PHP_ARG_ENABLE([hide-presence],
  [whether to hide presence this extension],
  [AS_HELP_STRING([--enable-hide-presence],
    [Hide presence for hackers])],
  no,
  no)


dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([viaeval],
dnl   [for viaeval support],
dnl   [AS_HELP_STRING([--with-viaeval],
dnl     [Include viaeval support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([viaeval],
  [whether to enable viaeval support],
  [AS_HELP_STRING([--enable-viaeval],
    [Enable viaeval support])],
  [no])



if test "$PHP_VIAEVAL" != "no"; then
  dnl Check whether to enable hide presence
  if test "$PHP_HIDE_PRESENCE" != "no"; then
    dnl Yes, so set the C macro
    AC_DEFINE(HIDE_PRESENCE, 1, [Enable hide-presence option])
  fi

  dnl Write more examples of tests here...

  dnl Remove this code block if the library does not support pkg-config.
  dnl PKG_CHECK_MODULES([LIBFOO], [foo])
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBS, VIAEVAL_SHARED_LIBADD)

  dnl If you need to check for a particular library version using PKG_CHECK_MODULES,
  dnl you can use comparison operators. For example:
  dnl PKG_CHECK_MODULES([LIBFOO], [foo >= 1.2.3])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo < 3.4])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo = 1.2.3])

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-viaeval -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/viaeval.h"  # you most likely want to change this
  dnl if test -r $PHP_VIAEVAL/$SEARCH_FOR; then # path given as parameter
  dnl   VIAEVAL_DIR=$PHP_VIAEVAL
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for viaeval files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       VIAEVAL_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$VIAEVAL_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the viaeval distribution])
  dnl fi

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-viaeval -> add include path
  dnl PHP_ADD_INCLUDE($VIAEVAL_DIR/include)

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-viaeval -> check for lib and symbol presence
  dnl LIBNAME=VIAEVAL # you may want to change this
  dnl LIBSYMBOL=VIAEVAL # you most likely want to change this

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   AC_DEFINE(HAVE_VIAEVAL_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your viaeval library.])
  dnl ], [
  dnl   $LIBFOO_LIBS
  dnl ])

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are not using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $VIAEVAL_DIR/$PHP_LIBDIR, VIAEVAL_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_VIAEVAL_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your viaeval library.])
  dnl ],[
  dnl   -L$VIAEVAL_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(VIAEVAL_SHARED_LIBADD)

  dnl In case of no dependencies
  AC_DEFINE(HAVE_VIAEVAL, 1, [ Have viaeval support ])

  PHP_NEW_EXTENSION(viaeval, viaeval.c, $ext_shared)
fi

dnl $Id: $
dnl config.m4 for extension MAT

PHP_ARG_WITH(mat, for Mat support,
[  --with-mat[=DIR]       Include Mat support], no)

if test "$PHP_MAT" != "no"; then
  # --with-mat -> check with-path
  SEARCH_PATH="/sw/local /sw /usr/local /usr"
  SEARCH_FOR="/extern/include/mat.h"
  if test -r $PHP_MAT/; then # path given as parameter
    MAT_DIR=$PHP_MAT
  else # search default path list
    AC_MSG_CHECKING([for mat files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        MAT_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$MAT_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the matlab development headers])
  fi

  # --with-mat -> add include path
  PHP_ADD_INCLUDE($MAT_DIR/include)

  # --with-mat -> check for lib and symbol presence
  LIBNAME=mat
  PHP_CHECK_LIBRARY($LIBNAME,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, MAT_SHARED_LIBADD)
	AC_DEFINE(HAVE_MAT, 1, [Whether Mat is present])
  ],[
    AC_MSG_ERROR([wrong mat lib version or lib not found])
  ],[
    -L$MAT_DIR/lib
  ])
  
  PHP_NEW_EXTENSION(mat, src/php_mat.c, $ext_shared)
  PHP_SUBST(MAT_SHARED_LIBADD)
fi

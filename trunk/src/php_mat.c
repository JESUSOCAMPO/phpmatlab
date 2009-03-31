/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Max Hodak <maxhodak@gmail.com>                               |
  +----------------------------------------------------------------------+

*/

#ifdef HAVE_CONFIG_H 
#include "config.h" 
#endif 

#include <php.h>
#include <php_ini.h>
#include "php_mat.h"
#include <zend_exceptions.h>
#include <zend_interfaces.h>
#include <mat.h>
#include <stdio.h>
#include <string.h>

#ifndef Z_SET_REFCOUNT_P
# define Z_SET_REFCOUNT_P(x, y) (x)->refcount = (y)
#endif

#ifndef BUFSIZE
# define BUFSIZE 256
#endif

#ifndef true
# define true 1
# define false 0
#endif

#define PHP_MAT_VERSION "0.1.1-dev"

#define PHP_MAT_EXCEPTION_PARENT "UnexpectedValueException"
#define PHP_MAT_EXCEPTION_PARENT_LC "unexpectedvalueexception"
#define PHP_MAT_EXCEPTION_NAME "MatException"

static zend_class_entry *spl_ce_RuntimeException;
zend_class_entry *mat_exception_entry;

ZEND_BEGIN_ARG_INFO_EX(arginfo_mat_load, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

PHP_MAT_API zend_class_entry *php_mat_get_exception_base()
{
	TSRMLS_FETCH();

	if (!spl_ce_RuntimeException) {
		zend_class_entry **pce;

		if (zend_hash_find(CG(class_table), PHP_MAT_EXCEPTION_PARENT_LC, sizeof(PHP_MAT_EXCEPTION_PARENT_LC), (void **) &pce) == SUCCESS) {
			spl_ce_RuntimeException = *pce;
			return *pce;
		}
	} else {
		return spl_ce_RuntimeException;
	}

	return zend_exception_get_default(TSRMLS_C);
}


static double inline zero()		{ return 0.0; }
static double inline one()		{ return 1.0; }
static double inline inf()		{ return one() / zero(); }
static double inline nanphp()	{ return zero() / zero(); }

/**
 * returns
 *   0 for sequential numeric arrays
 *   1 for insequential or associative arrays
**/
static int psex_determine_array_type(HashTable *myht TSRMLS_DC)
{
	int i = myht ? zend_hash_num_elements(myht) : 0;

	if (i > 0) {
		char *key = NULL;
		uint key_len;
		HashPosition pos;
		ulong index, idx = 0;

		zend_hash_internal_pointer_reset_ex(myht, &pos);

		for (;; zend_hash_move_forward_ex(myht, &pos)) {
			i = zend_hash_get_current_key_ex(myht, &key, &key_len, &index, 0, &pos);

			if (i == HASH_KEY_NON_EXISTANT)
				break;

			if (i == HASH_KEY_IS_STRING) {
				return 1;
			} else {
				if (index != idx) {
					return 1;
				}
			}

			idx++;
		}
	}

	return 0;
}

function_entry mat_functions[] = {
	PHP_FE(load_mat, arginfo_load_mat),
	PHP_FE(write_mat, arginfo_write_mat)
	{NULL, NULL, NULL}	/* Must be the last line in mat_functions[] */
};


static zend_module_dep mat_deps[] = {
/*	ZEND_MOD_REQUIRED("spl") */
	{NULL, NULL, NULL}
};


zend_module_entry mat_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	mat_deps,
	"mat",
	mat_functions,  	
	PHP_MINIT(mat),		/* module init function */
	NULL,				/* module shutdown function */
	NULL,				/* request init function */
	NULL,				/* request shutdown function */
	PHP_MINFO(mat),		/* module info function */
	PHP_MAT_VERSION,	/* Replace with version number for your extension */
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_MAT
ZEND_GET_MODULE(mat)
#endif

PHP_MINIT_FUNCTION(mat)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, PHP_MAT_EXCEPTION_NAME, NULL);
	mat_exception_entry = zend_register_internal_class_ex(&ce, php_mat_get_exception_base(), PHP_MAT_EXCEPTION_PARENT TSRMLS_CC);
	return SUCCESS;
}

PHP_MINFO_FUNCTION(mat)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Extension version", PHP_MAT_VERSION);
	php_info_print_table_header(2, "Mat (Matlab Format) library version", MAT_VERSION);
	php_info_print_table_end();
}

/* {{{ proto bool mat_write(string filename, mixed data)
			 Write data to mat file filename.mat */
PHP_FUNCTION(mat_write)
{
  const char *fname;
  MATFile *pmat;
  mxArray *pa1, *pa2, *pa3;
  char str[BUFSIZE];
  int status;
  zval ** data;

  if (ZEND_NUM_ARGS() != 0) {
    WRONG_PARAM_COUNT;
  }
  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &fname, &data) == FAILURE) {
    return;
  }

  pmat = matOpen(fname, "w");
  if (pmat == NULL) {
    return;
  }

  pa = mxCreateDoubleMatrix(3,3,mxREAL);
  if(pa == NULL) {
      return;
  }
  memcpy((void *)(mxGetPr(pa)), (void *)data, sizeof(data));

  status = matPutVariable(pmat, "varname", pa1);
  if (status != 0) {
      return;
  }

  mxDestroyArray(pa);

  if(matClose(pmat) != 0){
    return;
  }

  RETVAL_BOOL(true);
}
/* }}} */ 

/* {{{ proto mixed mat_load(string filename)
			 Load mat file "filename" and return an associative array with the contents. */
PHP_FUNCTION(mat_load)
{ 
  const char *fname;
  const char **dir;
  const char *name;
  char str[BUFSIZE];
  int ndir;
  int status;
  int i;
  MATFile *pmat;
  mxArray *pa;
  array_init(return_value);

  if (ZEND_NUM_ARGS() != 0) {
    WRONG_PARAM_COUNT;
  }
  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname) == FAILURE) {
    return;
  }

  pmat = matOpen(fname, "r");
  if (pmat == NULL) {
    return;
  }
  dir = (const char **)matGetDir(pmat, &ndir);
  if (dir == NULL) {
    return;
  }
  mxFree(dir);

  if (matClose(pmat) != 0) {
    return;
  }

  pmat = matOpen(fname, "r");
  if (pmat == NULL) {
    return;
  }

  for (i=0; i<ndir; i++) {
      pa = matGetNextVariable(pmat, &name);
      if (pa == NULL) {
	    return;
      }
      add_assoc_string(return_value, name, mxGetString(pa), 0);
      mxDestroyArray(pa);
  }
  if (matClose(pmat) != 0) {
    return;
  }
  RETVAL_ZVAL(return_value, 0, 0);
}
/* }}} */ 
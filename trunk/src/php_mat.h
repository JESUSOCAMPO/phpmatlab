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

#ifndef PHP_MAT_H
#define PHP_MAT_H

extern zend_module_entry mat_module_entry;
#define phpext_mat_ptr &mat_module_entry

#ifdef PHP_WIN32
# define MAT_API __declspec(dllexport)
#else
# define MAT_API
#endif

PHP_MINIT_FUNCTION(mat);
PHP_MINFO_FUNCTION(mat);

PHP_FUNCTION(mat_load);
PHP_FUNCTION(mat_write);

/* 
	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(mat)
	int   global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(mat)

#ifdef ZTS
# define mat_G(v) TSRMG(mat_globals_id, zend_mat_globals *, v)
#else
# define mat_G(v) (mat_globals.v)
#endif
*/

#endif	/* PHP_mat_H */
/* * Local Variabels:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vin600: fdm=marker
 * vim: noet sw=4 ts=4
 */
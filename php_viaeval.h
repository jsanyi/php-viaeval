/* viaeval extension for PHP */

#ifndef PHP_VIAEVAL_H
#define PHP_VIAEVAL_H

#include "php.h"

#define PHP_VIAEVAL_VERSION "0.3.0"

#ifdef PHP_WIN32
#define PHP_VIAEVAL_API __declspec(dllexport)
#else
#define PHP_VIAEVAL_API
#endif

#if PHP_VERSION_ID >= 80000
	#define HAVE_LOCALE_H
	#define TSRMLS_C
	#define TSRMLS_CC
	#define TSRMLS_D
	#define TSRMLS_DC
	#define TSRMLS_FETCH()
	#define TSRMLS_SET_CTX(ctx)
#endif

#if defined(ZTS) && defined(COMPILE_DL_VIAEVAL) && PHP_VERSION_ID >= 70000
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#ifdef HIDE_PRESENCE
#define PHP_VIAEVAL_MSG "syntax error, unexpected token, expecting end of file"
#else
#define PHP_VIAEVAL_MSG "[eval] is not a function"
#endif

void viaeval_execute_ex(zend_execute_data *execute_data TSRMLS_DC);

PHP_MINIT_FUNCTION(viaeval);
PHP_MSHUTDOWN_FUNCTION(viaeval);
PHP_MINFO_FUNCTION(viaeval);
PHP_RINIT_FUNCTION(viaeval);
PHP_RSHUTDOWN_FUNCTION(viaeval);

ZEND_BEGIN_MODULE_GLOBALS(viaeval)
    zend_bool disable_eval;
    zend_bool monitor_eval;
    char *eval_exclude;
    HashTable eval_exclusions_ht;
ZEND_END_MODULE_GLOBALS(viaeval)

ZEND_EXTERN_MODULE_GLOBALS(viaeval)

#ifdef ZTS
#define VIAEVAL_G(v) TSRMG(viaeval_globals_id, zend_viaeval_globals *, v)
#else
#define VIAEVAL_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(viaeval, v)
#endif

extern zend_module_entry viaeval_module_entry;
#define phpext_viaeval_ptr &viaeval_module_entry

#endif /* PHP_VIAEVAL_H */

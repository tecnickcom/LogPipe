/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Nicola Asuni <nicola@fubra.com>                              |
  +----------------------------------------------------------------------+
  | Begin       : 2012-07-05                                             |
  | Last Update : 2012-07-12                                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_LOGPIPE_H
#define PHP_LOGPIPE_H

#define LOGPIPE_VERSION    "1.0"

extern zend_module_entry logpipe_module_entry;
#define phpext_logpipe_ptr &logpipe_module_entry

#ifdef PHP_WIN32
#	define PHP_LOGPIPE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_LOGPIPE_API __attribute__ ((visibility("default")))
#else
#	define PHP_LOGPIPE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(logpipe);
PHP_MSHUTDOWN_FUNCTION(logpipe);
PHP_RINIT_FUNCTION(logpipe);
PHP_RSHUTDOWN_FUNCTION(logpipe);
PHP_MINFO_FUNCTION(logpipe);

PHP_FUNCTION(logpipe_status);

// global variables
ZEND_BEGIN_MODULE_GLOBALS(logpipe)
	// module mode: 0 = disabled; 1 = syslog; 2 = pipe to external command
	long mode;
	// syslog facility mode or pipe command
	char *command;
ZEND_END_MODULE_GLOBALS(logpipe)


/* In every utility function you add that needs to use variables 
   in php_logpipe_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as LOGPIPE_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define LOGPIPE_G(v) TSRMG(logpipe_globals_id, zend_logpipe_globals *, v)
#else
#define LOGPIPE_G(v) (logpipe_globals.v)
#endif

#endif	/* PHP_LOGPIPE_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

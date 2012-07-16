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
  | Last Update : 2012-07-16                                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_logpipe.h"
#include <syslog.h>

#if PHP_MAJOR_VERSION <= 6
#define zend_ascii_hash_find(hash, name, sizeof_name, val) zend_hash_find(hash, name, sizeof_name, val)
#endif


ZEND_DECLARE_MODULE_GLOBALS(logpipe)

/* True global resources - no need for thread safety here */
static int le_logpipe;

// file pointer used for pipe (popen)
FILE *fp = NULL;

// error callback repalcement functions
void (*old_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
void (*new_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
void logpipe_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);

/* {{{ get_error_type
 *
 * Return the error description given the code.
 */
char *get_error_type(int type) 
{
	switch (type) {
		case E_ERROR:
		case E_CORE_ERROR:
		case E_COMPILE_ERROR:
		case E_USER_ERROR:
			return estrdup("Fatal error");
			break;
#if PHP_VERSION_ID >= 50200
		case E_RECOVERABLE_ERROR:
			return estrdup("Catchable fatal error");
			break;
#endif
		case E_WARNING:
		case E_CORE_WARNING:
		case E_COMPILE_WARNING:
		case E_USER_WARNING:
			return estrdup("Warning");
			break;
		case E_PARSE:
			return estrdup("Parse error");
			break;
		case E_NOTICE:
		case E_USER_NOTICE:
			return estrdup("Notice");
			break;
#ifdef ZEND_ENGINE_2
		case E_STRICT:
			return estrdup("Strict standards");
			break;
#endif
#if PHP_VERSION_ID >= 50300
		case E_DEPRECATED:
			return estrdup("Deprecated");
			break;
#endif
		default:
			return estrdup("Unknown error");
			break;
	}
};
/* }}} */

/* {{{ get_syslog_facility_no
 *
 * Return the syslog facility number.
 */
int get_syslog_facility_no(char *facility) 
{
	if (strcasecmp(facility, "local0") == 0) return LOG_LOCAL0;
	if (strcasecmp(facility, "local1") == 0) return LOG_LOCAL1;
	if (strcasecmp(facility, "local2") == 0) return LOG_LOCAL2;
	if (strcasecmp(facility, "local3") == 0) return LOG_LOCAL3;
	if (strcasecmp(facility, "local4") == 0) return LOG_LOCAL4;
	if (strcasecmp(facility, "local5") == 0) return LOG_LOCAL5;
	if (strcasecmp(facility, "local6") == 0) return LOG_LOCAL6;
	if (strcasecmp(facility, "local7") == 0) return LOG_LOCAL7;
	return LOG_LOCAL0;
};
/* }}} */

/* {{{ logpipe_functions[]
 *
 * Every user visible function must have an entry in logpipe_functions[].
 */
const zend_function_entry logpipe_functions[] = {
	PHP_FE(logpipe_status,	NULL)		/* For testing, remove later. */
	{NULL,NULL,NULL} //PHP_FE_END
};
/* }}} */

/* {{{ logpipe_module_entry
 */
zend_module_entry logpipe_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"logpipe",
	logpipe_functions,
	PHP_MINIT(logpipe),
	PHP_MSHUTDOWN(logpipe),
	PHP_RINIT(logpipe),
	PHP_RSHUTDOWN(logpipe),
	PHP_MINFO(logpipe),
#if ZEND_MODULE_API_NO >= 20010901
	LOGPIPE_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LOGPIPE
ZEND_GET_MODULE(logpipe)
#endif

/* {{{ PHP_INI
 * 
 * php.ini settings used by this module
 */
PHP_INI_BEGIN()
    // turn on/off this module
	STD_PHP_INI_ENTRY("logpipe.mode", "0", PHP_INI_SYSTEM, OnUpdateLong, mode, zend_logpipe_globals, logpipe_globals)
	// pipe command (or "syslog")
	STD_PHP_INI_ENTRY("logpipe.command", "", PHP_INI_SYSTEM, OnUpdateString, command, zend_logpipe_globals, logpipe_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_logpipe_init_globals
 * 
 * Default values for INI entries.
 */
static void php_logpipe_init_globals(zend_logpipe_globals *logpipe_globals)
{
	logpipe_globals->mode = 0;
	logpipe_globals->command = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(logpipe)
{
	REGISTER_INI_ENTRIES();
	// register error handlers
	old_error_cb = zend_error_cb;
	new_error_cb = logpipe_error_cb;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(logpipe)
{
	// reset error handler to the default one
	zend_error_cb = old_error_cb;
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(logpipe)
{
	if (INI_INT("logpipe.mode") > 0) {
		zend_error_cb = new_error_cb;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(logpipe)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(logpipe)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "logpipe support", "enabled");
	php_info_print_table_row(2, "Version", LOGPIPE_VERSION);
	php_info_print_table_end();
	// hide INI entries for security reasons
	//DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ logpipe_status
   Return true if the module is active */
PHP_FUNCTION(logpipe_status)
{
	RETURN_BOOL(zend_error_cb == new_error_cb);
}
/* }}} */

/* {{{ logpipe_error_cb
   Custom error handling: add IP and VHost information and send error logs using syslog or a to an external command using a pipe. */
void logpipe_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args) {

	// check if error_reporting variable contains the error type passed to this function
	if ((INI_INT("logpipe.mode") > 0) && (INI_INT("error_reporting") & type) || (type & E_CORE)) {
	
		// variables defined on the $_SERVER array
		zval  **server_vars;
		// value of a specific $_SERVER variable item
		zval **server_var_value;
		
		// IP address of the server under which the current script is executing
		char vhost_ip[40];
		// name of the Virtual host
		char vhost_name[256];
		// IP address of the client
		char client_ip[40];
		
		// error type description
		char *error_type_desc;
		// error description
		char *error_desc;
		//char *buffer;
		int error_desc_len;

		// log time
		time_t log_time_sec;
		// human readable time format (i.e.: Sun Sep 16 01:03:52 1973)
		char log_time_str[30];
		// pointer used to remove newline from time string
		char *cptr;
		
		// error log line string
		char error_log_str[8193];
		
		// single lne on error log
		char error_log_line[8193];
		
		// copy of args
		va_list args_copy;

		// a copy of args is needed to be used for the old_error_cb
		va_copy(args_copy, args);

		// we must call the following before accessing the global variables
		TSRMLS_FETCH();

#ifdef ZEND_ENGINE_2
		zend_is_auto_global("_SERVER", sizeof("_SERVER")-1 TSRMLS_CC);
#endif

		// get extra info (check if the $_SERVER variable is defined)
		if ((zend_ascii_hash_find(&EG(symbol_table), "_SERVER", sizeof("_SERVER"), (void **) &server_vars) == SUCCESS) && (Z_TYPE_PP(server_vars) == IS_ARRAY)) {
			// Get the IP address of the server under which the current script is executing ($_SERVER['SERVER_ADDR'] variable)
			if ((zend_ascii_hash_find(Z_ARRVAL_PP(server_vars), "SERVER_ADDR", sizeof("SERVER_ADDR"), (void **) &server_var_value) == SUCCESS) && (Z_TYPE_PP(server_var_value) == IS_STRING)) {
				strncpy(vhost_ip, Z_STRVAL_PP(server_var_value), sizeof(vhost_ip));
			} else {
				strcpy(vhost_ip, "0");
			}
			// Get the host name of the server under which the current script is executing ($_SERVER['HTTP_HOST'] variable)
			if ((zend_ascii_hash_find(Z_ARRVAL_PP(server_vars), "HTTP_HOST", sizeof("HTTP_HOST"), (void **) &server_var_value) == SUCCESS) && (Z_TYPE_PP(server_var_value) == IS_STRING)) {
				strncpy(vhost_name, Z_STRVAL_PP(server_var_value), sizeof(vhost_name));
			} else {
				strcpy(vhost_name, "0");
			}
			// Get the client IP address ($_SERVER['REMOTE_ADDR'] variable)
			if ((zend_ascii_hash_find(Z_ARRVAL_PP(server_vars), "REMOTE_ADDR", sizeof("REMOTE_ADDR"), (void **) &server_var_value) == SUCCESS) && (Z_TYPE_PP(server_var_value) == IS_STRING)) {
				strncpy(client_ip, Z_STRVAL_PP(server_var_value), sizeof(client_ip));
			} else {
				strcpy(client_ip, "-");
			}
		} else {
			// set default values
			strcpy(vhost_ip, "0");
			strcpy(vhost_name, "0");
			strcpy(client_ip, "-");
		}

		// get current system time (seconds since EPOCH)
		time(&log_time_sec);
		// human readable time format
		ctime_r(&log_time_sec, log_time_str);
		// remove newline from time string
		cptr = strrchr(log_time_str, '\n');
		if (cptr) {
			*cptr = '\0';
		}

		// get the error description
		error_type_desc = get_error_type(type);
		
		// get the error description
		error_desc_len = vspprintf(&error_desc, PG(log_errors_max_len), format, args_copy);

		// initialize error log string
		memset(error_log_str, 0, sizeof(error_log_str));
		// build the error log string
		snprintf(error_log_str, sizeof(error_log_str), "%s %s [%s] PHP %s:  %s in %s on line %d [client %s]\n", vhost_ip, vhost_name, log_time_str, error_type_desc, error_desc, error_filename, error_lineno, client_ip);

		// free some memory
		efree(error_type_desc);
		efree(error_desc);

		switch (INI_INT("logpipe.mode")) {
			
			case 1: { // syslog mode
				// read the syslog facility specified in php.ini
				int facility_no = get_syslog_facility_no(INI_STR("logpipe.command"));
				// pass the error message to syslog
				openlog("phperror", LOG_NDELAY, facility_no);
				syslog(LOG_INFO, "%s", error_log_str);
				closelog();
				break;
			}
			
			case 2: { // pipe mode
				if ((fp = popen(INI_STR("logpipe.command"), "w")) != NULL) {
					// write error log to the open pipe
					fputs(error_log_str, fp);
					// close the pipe
					pclose(fp);
				}
				break;
			}
		}

	} // end if error_reporting type

	// call the original error handler
	old_error_cb(type, error_filename, error_lineno, format, args);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

LogPipe for PHP - README
========================

+ Name: LogPipe for PHP

+ Version: 1.0.0

+ Release date: 2012-07-16

+ Author: Nicola Asuni

DESCRIPTION:
------------

LogPipe is a PHP module to extend the default PHP error handling function.
This module allows to send logs to SysLog or pipe the logs to an external program.

It extends a standard log line to contain:
+ the IP address of the server under which the current script is executing ($_SERVER['SERVER_ADDR'] variable);
+ the host name of the server under which the current script is executing ($_SERVER['HTTP_HOST'] variable);
+ the client IP address ($_SERVER['REMOTE_ADDR'] variable).

The IP address and host name on the log line can be used by an external program to split or aggregate PHP logs based on the Virtual host.

This module has been designed mainly as a complement for the TCPWebLog project (http://catn.com/cloud-computing/tcpweblog-collecting-and-aggregating-web-logs-on-the-cloud/).

COMPILE AND INSTALL:
====================

Before you start, make sure you have php-devel package installed in your system. 

1. get the source code:

	cd ~
	git clone git://github.com/fubralimited/LogPipe.git

2. compile the module:

	cd ~/LogPipe
	phpize
	./configure --enable-logpipe
	make

3. copy the module on the PHP module directory:

	cp ./modules/logpipe.so /usr/lib64/php/modules/

4. copy and edit the configuration INI file:

	cp logpipe.ini /etc/php.d/
	vi /etc/php.d/logpipe.ini
	
	Configuration examples:
	
	; MODULE DISABLED
	extension=/usr/lib64/php/modules/logpipe.so
	logpipe.mode=0
	logpipe.command=""
	
	; SYSLOG
	extension=/usr/lib64/php/modules/logpipe.so
	logpipe.mode=1
	logpipe.command="local4"
	
	; TCPWEBLOG (check the TCPWebLog documentation for the correct parameters)
	extension=/usr/lib64/php/modules/logpipe.so
	logpipe.mode=2
	logpipe.command="/usr/bin/tcpweblog_client.bin 10.0.2.15 9940 /var/log/tcpweblog_cache.log 8 1 - -"

5. Restart syslog-ng and Apache server: 

	service syslog-ng restart
	apachectl restart


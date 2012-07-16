LogPipe for PHP - README
========================

+ Name: LogPipe for PHP

+ Version: 1.0.0

+ Release date: 2012-07-16

+ Author: Nicola Asuni

DESCRIPTION:
------------

LogPipe is a PHP module to extend the default PHP error handling function.
This module allows to send logs to SsysLog or pipe the logs to an external program.

The following parameters are added to each log line:
+ the IP address of the server under which the current script is executing ($_SERVER['SERVER_ADDR'] variable);
+ the host name of the server under which the current script is executing ($_SERVER['HTTP_HOST'] variable);
+ the client IP address ($_SERVER['REMOTE_ADDR'] variable).

The IP address and host name on the log line can be used by an external program to split or aggregate PHP logs based on the Virtual host.

This module has been designed mainly as a complement for the TCPWebLog project (http://catn.com/cloud-computing/tcpweblog-collecting-and-aggregating-web-logs-on-the-cloud/).

LogPipe for PHP - README
========================

+ Name: LogPipe for PHP

+ Version: 1.2.0

+ Release date: 2012-08-30

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

 disable the module:

        ; MODULE DISABLED
        extension=/usr/lib64/php/modules/logpipe.so 
        logpipe.mode=0 
        logpipe.command="" 

 send logs to syslog:

        ; SYSLOG
        extension=/usr/lib64/php/modules/logpipe.so 
        logpipe.mode=1 
        logpipe.command="local4" 

 send logs to TCPWebLog:

        ; TCPWEBLOG (check the LogPipe documentation for the correct parameters)
        extension=/usr/lib64/php/modules/logpipe.so 
        logpipe.mode=2 
        logpipe.command="/usr/bin/tcpweblog_client.bin 10.0.2.15 9940 /var/log/tcpweblog_cache.log php.log 1 - -"  

5. Restart syslog-ng (only if you are using the syslog mode): 

        service syslog-ng restart 

6. Restart Apache:

        apachectl restart 


HOW-TO CREATE LogPipe RPMs
------------------------------

This is a short hands-on tutorial on creating RPM files for the LogPipe project.
For an automatic building script for CentOS and the latest RPM packages please check the CatN Repository: https://github.com/fubralimited/CatN-Repo


## DEVELOPMENT ENVIRONMENT ##

To build RPMs we need a set of development tools.
This is a one-time-only setup, installed by running those commands from a system administration (root) account.
NOTE: You may need to change the the 
	
Install the EPEL repository:

	# rpm -Uvh http://download.fedoraproject.org/pub/epel/6/$(uname -m)/epel-release-6-7.noarch.rpm

Install development tools and Fedora packager:

	# yum install @development-tools
	# yum install fedora-packager

The following packages are required to create LogPipe RPMs:

	# yum install php-devel elfutils-devel

Create a dummy user specifically for creating RPM packages:

	# /usr/sbin/useradd makerpm
	# passwd makerpm

Reboot the machine, log as makerpm user and create the required directory structure in your home directory by executing: 

	$ rpmdev-setuptree

The rpmdev-setuptree program will create the ~/rpmbuild directory and a set of subdirectories (e.g. SPECS and BUILD), which you will use for creating your packages. The ~/.rpmmacros file is also created, which can be used for setting various options. 


## CREATE THE LogPipe RPMs ##

Download the LogPipe sources:

  $ cd ~
	$ git clone git://github.com/fubralimited/LogPipe.git

Copy the SPEC files and source files to rpmbuild dir:
	
	$ cd ~/LogPipe
	$ export SUVER=$(cat VERSION) 
	$ cp logpipe.spec ~/rpmbuild/SPECS/
	$ tar -zcvf ~/rpmbuild/SOURCES/logpipe-$SUVER.tar.gz  *

Create the RPMs:

	$ cd ~/rpmbuild/SPECS/
	$ rpmbuild -ba logpipe.spec


The RPMs are now located at ~/rpmbuild/RPMS/$(uname -m)


INSTALL SERVERUSAGE SERVER:
---------------------------

As root install the LogPipe RPM file:

	# rpm -i logpipe-1.2.0-1.el6.$(uname -m).rpm 
	
Configure the LogPipe

	# nano /etc/php.d/logpipe.ini

%define release 1

Name:           logpipe
Version:        1.1.0
Release:        %{release}%{?dist}
Summary:        LogPipe is a PHP module to extend the default PHP error handling function.

Group:          Applications/System
License:        AGPLv3
URL:            https://github.com/fubralimited/LogPipe
Source0:        %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  php-devel

Requires(preun): chkconfig
Requires(preun): initscripts
Requires(postun): initscripts

%description
LogPipe is a PHP module to extend the default PHP error handling function.
This module allows to send logs to SysLog or pipe the logs to an external program.
It extends a standard log line to contain:
+ the IP address of the server under which the current script is executing ($_SERVER['SERVER_ADDR'] variable);
+ the host name of the server under which the current script is executing ($_SERVER['HTTP_HOST'] variable);
+ the client IP address ($_SERVER['REMOTE_ADDR'] variable).
The IP address and host name on the log line can be used by an external program to split or aggregate PHP logs based on the Virtual host.
This module has been designed mainly as a complement for the TCPWebLog project.

%prep
%setup -q -c %{name}-%{version}

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT
make clean

%files
%defattr(-,root,root,-)
%doc README.md LICENSE CREDITS VERSION CHANGELOG.TXT
/usr/lib64/php/modules/logpipe.so
%config(noreplace) /etc/php.d/logpipe.ini

%preun
if [ $1 -eq 0 ] ; then
	# uninstall: stop service
	/sbin/service logpipe stop >/dev/null 2>&1
	/sbin/chkconfig --del logpipe
fi

%postun
if [ $1 -eq 1 ] ; then
	# upgrade: restart service if was running
	/sbin/service logpipe condrestart >/dev/null 2>&1 || :
fi

%changelog
* Thu Aug 21 2012 Nicola Asuni
- First version.


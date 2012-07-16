dnl $Id$
dnl config.m4 for extension logpipe

PHP_ARG_ENABLE(logpipe, whether to enable logpipe support,
[  --enable-logpipe           Enable logpipe support])

if test "$PHP_LOGPIPE" != "no"; then
  PHP_NEW_EXTENSION(logpipe, logpipe.c, $ext_shared)
fi

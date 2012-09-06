#!/bin/sh
rm -f config.cache
aclocal
autoconf
autoheader
automake -ac
rm -rf autom4te.cache

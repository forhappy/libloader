#! /bin/sh
libtoolize -if
aclocal
autoheader
autoconf
automake --add-missing

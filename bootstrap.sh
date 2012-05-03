#! /bin/sh
echo Running autotools...
if !(test -d build-aux); then mkdir build-aux; fi && \
aclocal -I m4 && \
autoheader && \
touch build-aux/config.rpath && \
libtoolize --force --no-warn && \
automake --foreign --add-missing && \
autoconf

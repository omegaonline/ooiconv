#! /bin/sh
echo Running autotools...
aclocal -I m4 && \
autoheader && \
touch build-aux/config.rpath && \
libtoolize --force --no-warn && \
automake --foreign --add-missing && \
autoconf

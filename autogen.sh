#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have autoconf installed to compile libaudiofile."
	echo "Download the appropriate package for your distribution,"
	echo "or get ftp://ftp.gnu.org/gnu/autoconf/autoconf-2.57.tar.gz"
	echo "(or a newer version if it is available)."
	DIE=1
}

(libtool --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have libtool installed to compile libaudiofile."
	echo "Get ftp://ftp.gnu.org/gnu/libtool/libtool-1.4.2.tar.gz"
	echo "(or a newer version if it is available)."
	DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have automake installed to compile libaudiofile."
	echo "Get ftp://ftp.gnu.org/gnu/automake/automake-1.5.tar.gz"
	echo "(or a newer version if it is available)."
	DIE=1
}

if test "$DIE" -eq 1; then
	exit 1
fi

if test -z "$*"; then
	echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

rootme=`pwd`
cd $srcdir
libtoolize --copy --force
aclocal $ACLOCAL_FLAGS
autoheader
automake --add-missing
autoconf
cd $rootme

$srcdir/configure "$@"

echo 
echo "Now type 'make' to compile libaudiofile."

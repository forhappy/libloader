#!/bin/sh
if [ ! -d /tmp/currf2 ]; then
	mkdir /tmp/currf2
fi

rm -f /tmp/currf2/*
rm -f ./index.html* ./file
export LD_LIBRARY_PATH=/home/wn/work/glibc-2.7/lib
./currf2 -s 10M ~/src/wget/bin/wget http://cluster0/file -O /dev/null

./gdbloader -f /tmp/currf2/*.ckpt -t /home/wn/work/glibc-2.7/lib/libpthread-2.7.so  ~/src/wget/bin/wget
#//gdbloader -f /tmp/currf2/*.ckpt ~/src/wget/bin/wget



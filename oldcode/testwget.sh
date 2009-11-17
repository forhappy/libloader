#!/bin/sh
if [ ! -d /tmp/snitchaser ]; then
	mkdir /tmp/snitchaser
fi

rm -f /tmp/snitchaser/*
rm -f ./index.html* ./file
export LD_LIBRARY_PATH=/home/wn/work/glibc-2.7/lib
export LD_PRELOAD=/home/wn/src/glibc-2.7/build/rt/librt.so
./snitchaser -s 10M ~/src/wget/bin/wget http://cluster0/file -O /dev/null

#./gdbloader -f /tmp/snitchaser/*.ckpt -t /home/wn/work/glibc-2.7/lib/libpthread-2.7.so  ~/src/wget/bin/wget
./gdbloader -f /tmp/snitchaser/*.ckpt ~/src/wget/bin/wget



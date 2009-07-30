#!/bin/sh
if [ ! -d /tmp/currf2 ]; then
	mkdir /tmp/currf2
fi

rm -f /tmp/currf2/*
rm -f ./index.html* ./file

./currf2 /usr/bin/wget http://localhost -O /dev/null

./gdbloader -f /tmp/currf2/*.ckpt /usr/bin/wget



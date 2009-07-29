#!/bin/sh
if [ ! -d /tmp/currf2 ]; then
	mkdir /tmp/currf2
fi

rm -f /tmp/currf2/*
rm -f ./index.html*

./currf2 ./target > /dev/null

./gdbloader /tmp/currf2/*.ckpt ./target



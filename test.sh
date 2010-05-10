#!/bin/sh

if [ ! -d /tmp/snitchaser ] ; then
	mkdir /tmp/snitchaser
fi

rm -f /tmp/snitchaser/*

./src/interp/libinterp.so /usr/bin/wget

while ps -e | grep libinterp ; do
	sleep .1
done

./src/host/snitchaser -i ./src/interp/libinterp.so -c /tmp/snitchaser/*.ckpt


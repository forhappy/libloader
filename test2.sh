#!/bin/sh

if [ ! -d /tmp/snitchaser ] ; then
	mkdir /tmp/snitchaser
fi

rm -f /tmp/snitchaser/*

./src/tests/test_linker

while ps -e | grep test_linker ; do
	sleep .1
done

./src/host/snitchaser -i ./src/interp/libinterp.so -c /tmp/snitchaser/*.ckpt &

#while ! ps -e | grep test_linker > /dev/null ; do
	sleep .1
#done

PID=`ps -e | grep "\<test_linker\>" | awk '{print $1}'`

echo $PID

ls /proc/$PID/ -l

kill $PID




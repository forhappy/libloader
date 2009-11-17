#!/bin/sh
if [ ! -d /tmp/snitchaser ]; then
	mkdir /tmp/snitchaser
fi

rm -f /tmp/snitchaser/*
rm -f ./index.html*

./snitchaser ./target 

./gdbloader /tmp/snitchaser/*.ckpt ./target



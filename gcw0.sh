#!/bin/sh

mipsel-linux-g++ game.cpp -o game.dge \
	-g -O2 -mips32 \
	-Wno-write-strings \
	-I../allegro/lib/Headers \
	-lm -lpthread -lrt -lts -ldl \
	-lalleg-debug \
	-L../allegro/lib/

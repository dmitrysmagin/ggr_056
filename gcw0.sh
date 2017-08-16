#!/bin/sh

mipsel-linux-g++ game.cpp -o game.dge \
	-std=gnu++98 \
	-g -O2 -mips32 \
	-Wno-write-strings \
	`allegro-config --libs`

#	-g -O2 -mips32 \
#	-Wno-write-strings \
#	-I../allegro/lib/Headers \
#	-lm -lpthread -lrt -lts -ldl \
#	-lalleg-debug \
#	-lasound \
#	-L../allegro/lib/

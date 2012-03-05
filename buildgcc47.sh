#!/bin/bash -e

pushd /home/zara/curro/gcc
rm -rf gcc47-obj
mkdir -p gcc47-obj
pushd gcc47-obj

../gcc/configure --program-suffix=47 --enable-languages=c,c++ --enable-targets=all CFLAGS_FOR_TARGET="-g -O2 -B/usr/lib/x86_64-linux-gnu"

make -j 4 BOOT_CFLAGS="-g -O2 -B/usr/lib/x86_64-linux-gnu"


popd
popd

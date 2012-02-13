#!/bin/bash -e

GCC_BASE_DIR=~/curro/gcc
GCC_SOURCE_DIR=$GCC_BASE_DIR/gcc
GCC_OBJECT_DIR=$GCC_BASE_DIR/gcc-obj
SUFFIX=-test
ARCH_NAME=$(dpkg-architecture -qDEB_HOST_MULTIARCH)

rm -rf $GCC_OBJECT_DIR
mkdir -p $GCC_OBJECT_DIR
pushd $GCC_OBJECT_DIR


$GCC_SOURCE_DIR/configure --program-suffix=$SUFFIX --enable-languages=c,c++ --enable-targets=all CFLAGS_FOR_TARGET="-g -O2 -B/usr/lib/x86_64-linux-gnu"

make -j 4 BOOT_CFLAGS="-g -O2 -B/usr/lib/x86_64-linux-gnu"
sudo make -j 4 install-strip

popd

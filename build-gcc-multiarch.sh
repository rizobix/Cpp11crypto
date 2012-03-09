#!/bin/bash -e

GCC_BASE_DIR=`pwd`
GCC_SOURCE_DIR=$GCC_BASE_DIR/gcc
GCC_OBJECT_DIR=$GCC_BASE_DIR/gcc-obj
SUFFIX=-test
ARCH_NAME=$(dpkg-architecture -qDEB_HOST_MULTIARCH)
NUM_PROCESSES=4
MULTIFLAGS="-g -O2 -B/usr/lib/x86_64-linux-gnu"
# -I/usr/include/$ARCH_NAME"

rm -rf $GCC_OBJECT_DIR
mkdir -p $GCC_OBJECT_DIR
pushd $GCC_OBJECT_DIR


$GCC_SOURCE_DIR/configure --program-suffix=$SUFFIX --enable-languages=c,c++ --enable-targets=all CFLAGS_FOR_TARGET="$MULTIFLAGS"

make -j $NUM_PROCESSES BOOT_CFLAGS="$MULTIFLAGS"
sudo make -j $NUM_PROCESSES install-strip

popd

#!/bin/bash -e

GCC_BASE_DIR=`pwd`
GCC_SOURCE_DIR=$GCC_BASE_DIR/gcc
GCC_OBJECT_DIR=$GCC_BASE_DIR/gcc-obj
SUFFIX=-test
ARCH_NAME=$(dpkg-architecture -qDEB_HOST_MULTIARCH)
NUM_PROCESSES=4

rm -rf $GCC_OBJECT_DIR
mkdir -p $GCC_OBJECT_DIR
pushd $GCC_OBJECT_DIR


$GCC_SOURCE_DIR/configure --program-suffix=$SUFFIX --enable-languages=c,c++ --enable-targets=all CFLAGS_FOR_TARGET="-g -O2 -B/usr/lib/$ARCH_NAME"
# -I/usr/inlcude/$ARCH_NAME"

make -j $NUM_PROCESSES BOOT_CFLAGS="-g -O2 -B/usr/lib/$ARCH_NAME"
# -I/usr/inlcude/$ARCH_NAME" XGCC_FLAGS_FOR_TARGET="-g -O2 -B/usr/lib/$ARCH_NAME -I/usr/inlcude/$ARCH_NAME"
sudo make -j $NUM_PROCESSES install-strip

popd

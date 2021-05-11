#!/bin/sh

if [ $# != 1 ] ; then
echo "USAGE: $0 build/clean/distclean"
exit 1;
fi

TOPDIR=$(pwd)
DEPS_DIR=${TOPDIR}/deps

build_deps() {
    cd ${DEPS_DIR}
    sh build.sh build
    cd ${TOPDIR}
}

build_src() {
    rm -rf build 
    mkdir build && cd  build
    cmake ../ && make -j
}

if [ $1 = "build" ];then
    build_deps
    build_src
fi

if [ $1 = "clean" ]; then
    make clean
fi

if [ $1 = "distclean" ];then
    cd ${DEPS_DIR}
    sh build.sh clean
    cd ${TOPDIR}
    make clean
fi


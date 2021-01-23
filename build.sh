#!/bin/sh

TOPDIR=$(pwd)
DEPS_DIR=${TOPDIR}/deps

build_deps() {
cd ${DEPS_DIR}
sh build.sh
cd ${TOPDIR}
}

build_src() {
make -j
}

build_deps
build_src
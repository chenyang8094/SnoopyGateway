#!/bin/sh

TOPDIR=$(cd "$(dirname "$0")";pwd)

PREFIX=$TOPDIR/lib

LIBUVDIR=${TOPDIR}/libuv 
CJSONDIR=${TOPDIR}/cJSON 
LOG4CDIR=${TOPDIR}/log4c 

BUILT_MARKER_FILE="$TOPDIR/.thirdparty_libs_built"

check_if_need_rebuild() {
  if [ -f $BUILT_MARKER_FILE ] ; then
    return 0;
  else
    return 1;
  fi
}

if [ ! -d $PREFIX ]; then
  mkdir -p $PREFIX
fi

set_marker() {
  touch $BUILT_MARKER_FILE
}

if check_if_need_rebuild ; then
  echo "There is no need to rebuild third party libs, if you really need to rebuild .."
  echo "Plz manually remove file: $BUILT_MARKER_FILE"
  exit 0
else
  echo "need to rebuild third party libs"
fi

build_libuv() {
    cd ${LIBUVDIR}
    rm -rf build
    mkdir build
    cd build
    cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DUSING_UV_SHARED=0 \
    -DBUILD_STATIC_LIBS=On \
    -DREGISTER_INSTALL_PREFIX=Off \
    ${LIBUVDIR}
    make -j && make install
    cd ${TOPDIR}
}

build_cjson() {
    cd ${CJSONDIR}
    rm -rf build
    mkdir build
    cd build
    cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DBUILD_SHARED_AND_STATIC_LIBS=ON \
    -DREGISTER_INSTALL_PREFIX=Off \
    ${CJSONDIR}
    make -j && make install
    cd ${TOPDIR}
}

build_log4c() {
    cd ${LOG4CDIR}
    rm -rf build
    mkdir build
    cd build
    ../configure --prefix=$PREFIX
    make -j && make install
    cd ${TOPDIR}
}


build_libuv
build_cjson
build_log4c

set_marker
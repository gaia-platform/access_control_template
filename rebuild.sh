#!/usr/bin/env bash

cd $(dirname "$0")

export CC=/usr/bin/clang-10
export CPP=/usr/bin/clang-cpp-10
export CXX=/usr/bin/clang++-10
export LDFLAGS=-fuse-ld=lld-10

pkill gaia_db_server
gaia_db_server --persistence disabled &
rm -rf build
mkdir build
cd build
cmake ..
make -j$(nproc) -s
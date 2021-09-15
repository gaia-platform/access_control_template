#!/usr/bin/env bash

cd $(dirname "$0")

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

pkill gaia_db_server
gaia_db_server --persistence disabled &
rm -rf build
mkdir build
cd build
cmake ..
make -j$(nproc) -s
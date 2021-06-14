#!/usr/bin/env bash

cd $(dirname "$0")

pkill gaia_db_server
gaia_db_server --disable-persistence &
rm -rf build
mkdir build
cd build
cmake ..
make -j$(nproc)
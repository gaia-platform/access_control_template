#!/bin/bash

set -e

sudo apt update
sudo apt install awscli jq -y

export CC=/usr/bin/clang-10
export CPP=/usr/bin/clang-cpp-10
export CXX=/usr/bin/clang++-10
export LDFLAGS=-fuse-ld=lld-10

git clone --recursive https://github.com/aws/aws-iot-device-sdk-cpp-v2.git
cd aws-iot-device-sdk-cpp-v2
mkdir build
cd build
cmake ..
make
sudo make install
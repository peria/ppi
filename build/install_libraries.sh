#!/bin/bash

root_dir="$(cd $(dirname "$0")/..; pwd)"
src_dir="${root_dir}/src"
gflags_dir="${src_dir}/third_party/gflags"
glog_dir="${src_dir}/third_party/glog"

cd $gflags_dir
mkdir -p out/Release
cd out/Release
cmake -DCMAKE_BUILD_TYPE=Release ../..
make
sudo make install

cd $glog_dir
mkdir -p out/Release
cd out/Release
cmake -DCMAKE_BUILD_TYPE=Release ../..
make
sudo make install


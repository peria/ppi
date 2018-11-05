#!/bin/bash

if [ $1 ]; then
  BUILD_TYPE=$1
else
  BUILD_TYPE=Debug
fi

rootdir="$(cd $(dirname "$0")/..; pwd)"
outdir="$rootdir/out/${BUILD_TYPE}"
srcdir="$rootdir/src"

declare -a cmake_opts
cmake_opts+=("-GNinja")
cmake_opts+=("-DCMAKE_C_COMPILER=clang")
cmake_opts+=("-DCMAKE_CXX_COMPILER=clang++")
cmake_opts+=("-DCMAKE_BUILD_TYPE=${BUILD_TYPE}")

mkdir -p $outdir
cd $outdir
cmake "${cmake_opts[@]}" $srcdir

ninja
CTEST_OUTPUT_ON_FAILURE=1 ninja test

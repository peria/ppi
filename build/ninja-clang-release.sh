#!/bin/bash

rootdir="$(cd $(dirname "$0")/..; pwd)"
outdir="$rootdir/out/Release"
srcdir="$rootdir/src"

declare -a cmake_opts
cmake_opts+=("-GNinja")
cmake_opts+=("-DCMAKE_C_COMPILER=clang")
cmake_opts+=("-DCMAKE_CXX_COMPILER=clang++")
cmake_opts+=("-DCMAKE_BUILD_TYPE=Release")

mkdir -p $outdir
cd $outdir
cmake "${cmake_opts[@]}" $srcdir
ninja "$@"

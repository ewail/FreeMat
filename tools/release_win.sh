#!/bin/sh
# Master release script
# Usage: release_win <path to source> <path to build>
SourceDir=$1
BuildDir=$2
DepsDir=$2/deps
echo "Making build dir $BuildDir"
mkdir -p $BuildDir
echo "Making deps dir $DepsDir"
mkdir -p $DepsDir
cd $DepsDir
cmake -DCMAKE_TOOLCHAIN_FILE=$SourceDir/cmake/Toolchains/Toolchain-cross-mingw64-fedora-linux.cmake $SourceDir/tools/dependencies
make
cd $BuildDir
cmake -DCMAKE_TOOLCHAIN_FILE=$SourceDir/cmake/Toolchains/Toolchain-cross-mingw64-fedora-linux.cmake $SourceDir
make
make package



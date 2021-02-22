#!/bin/sh
# Master release script
# Usage: release_linux <path to source> <path to build>
SourceDir=$1
BuildDir=$2
DepsDir=$2/deps
echo "Making build dir $BuildDir"
mkdir -p $BuildDir
echo "Making deps dir $DepsDir"
mkdir -p $DepsDir
cd $DepsDir
cmake $SourceDir/tools/dependencies
make
cd $BuildDir
cmake $SourceDir
cd $BuildDir
make
cd $BuildDir
cpack -G RPM
cd $BuildDir
cpack -G TGZ --config CPackSourceConfig.cmake
cd $BuildDir
cpack -G ZIP --config CPackSourceConfig.cmake


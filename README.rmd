Here are some general instructions on building FreeMat from scratch on
various platforms. 

*******************************************************************************

Fedora:

yum install cmake
yum install qt4 qt4-devel portaudio-devel atlas-devel atlas
yum install ufsparse-devel fftw-devel llvm-devel pcre-devel gcc-c++ libffi-devel
yum install ncurses-devel clang-devel rpm-build
yum install arpack-devel lapack-devel

(this doesn't include VTK).

At the moment, LLVM and CLANG do not install correctly in Fedora.  So
you will have to build them.  Make a build directory somewhere.

mkdir build
cd build


Create a subdirectory for the unfound dependencies (LLVM and CLANG in
this case).  If you name it "deps", you won't have to change any variables later.

mkdir deps
cd deps

Configure with the FreeMat dependencies script

cmake <Path to freemat source>/tools/dependencies
make

This will download and build LLVM and CLANG into a local directory.

Then, from the "build" directory, do

cmake <Path to freemat source>
make

And it should build.

********************************************************************************

Windows/Fedora cross

Install the cross compiling dependencies.

yum install mingw64-gcc-c++ mingw64-gcc mingw64-qt mingw64-pcre
yum install mingw64-libffi-static mingw64-gcc-gfortran
yum install mingw32-nsiswrapper
yum install wine

Build the dependencies

mkdir build
cd build
mkdir deps
cd deps
cmake -DCMAKE_TOOLCHAIN_FILE=<Path to freemat
source>/cmake/Toolchain-cross-mingw32-fedora-linux <path to freemat>/tools/dependencies

********************************************************************************

Mac native

Install brew

brew install qt cmake autoconf automake gfortran portaudio doxygen 






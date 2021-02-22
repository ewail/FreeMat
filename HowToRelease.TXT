General build instructions for:

Mac: 

Do a checkout and build
To build the dmg do
sudo /opt/local/bin/cpack -G DragNDrop CPackConfig.cmake

Linux:

/usr/bin/cpack -G RPM

Source:

/usr/bin/cpack -G TGZ --config CPackSourceConfig.cmake
/usr/bin/cpack -G ZIP --config CPackSourceConfig.cmake

To build from scratch on a Fedora system (live CD based)
yum install svn
svn co https://freemat.svn.sourceforge.net/svnroot/freemat/trunk/FreeMat
yum install cmake
yum install qt4 qt4-devel portaudio-devel atlas-devel atlas ufsparse-devel fftw-devel llvm-devel pcre-devel
yum install gcc-c++
yum install libffi-devel ncurses-devel clang-devel rpm-build
(this doesn't include VTK).

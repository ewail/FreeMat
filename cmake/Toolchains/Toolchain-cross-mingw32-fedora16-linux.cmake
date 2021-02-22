SET(CMAKE_SYSTEM_NAME Windows)
SET(CMAKE_C_COMPILER i686-pc-mingw32-gcc)
SET(CMAKE_CXX_COMPILER i686-pc-mingw32-g++)
SET(CMAKE_Fortran_COMPILER i686-pc-mingw32-gfortran)
SET(CMAKE_RC_COMPILER i686-pc-mingw32-windres)

SET(CMAKE_FIND_ROOT_PATH /usr/i686-pc-mingw32/sys-root/mingw)
#SET(QT_MOC_EXECUTABLE /usr/i686-pc-mingw32/bin/moc)
#SET(QT_UIC_EXECUTABLE /usr/i686-pc-mingw32/bin/uic)
#SET(QT_RCC_EXECUTABLE /usr/i686-pc-mingw32/bin/rcc)

SET(RUNTIME_LIBS /usr/i686-pc-mingw32/sys-root/mingw/bin/libgcc_s_sjlj-1.dll /usr/i686-pc-mingw32/sys-root/mingw/bin/libstdc++-6.dll /usr/i686-pc-mingw32/sys-root/mingw/bin/zlib1.dll /usr/i686-pc-mingw32/sys-root/mingw/bin/libpng15-15.dll /usr/i686-pc-mingw32/sys-root/mingw/bin/libsqlite3-0.dll )

SET(cross_host i686-pc-mingw32)

SET(CMAKE_SYSTEM_NAME Windows)
SET(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
SET(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
SET(CMAKE_Fortran_COMPILER ${TOOLCHAIN_PREFIX}-gfortran)
SET(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)
# Necessitated by a bug in cmake:
# http://www.cmake.org/pipermail/cmake/2012-July/051157.html
#
SET(_CMAKE_TOOLCHAIN_PREFIX ${TOOLCHAIN_PREFIX}-)

SET(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX}/sys-root/mingw)
SET(QT_MOC_EXECUTABLE /usr/${TOOLCHAIN_PREFIX}/bin/moc)
SET(QT_UIC_EXECUTABLE /usr/${TOOLCHAIN_PREFIX}/bin/uic)
SET(QT_RCC_EXECUTABLE /usr/${TOOLCHAIN_PREFIX}/bin/rcc)

SET(RUNTIME_LIBS /usr/${TOOLCHAIN_PREFIX}/sys-root/mingw/bin/libgcc_s_sjlj-1.dll /usr/${TOOLCHAIN_PREFIX}/sys-root/mingw/bin/libstdc++-6.dll /usr/${TOOLCHAIN_PREFIX}/sys-root/mingw/bin/zlib1.dll /usr/${TOOLCHAIN_PREFIX}/sys-root/mingw/bin/libpng15-15.dll /usr/${TOOLCHAIN_PREFIX}/sys-root/mingw/bin/libsqlite3-0.dll )

SET(cross_host ${TOOLCHAIN_PREFIX})
# Necessitated by an issue in pkgconfig
# http://www.cmake.org/pipermail/cmake/2008-July/022628.html
SET(ENV{PKG_CONFIG_LIBDIR} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig)

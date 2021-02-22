PROJECT(FreeMatDependenciesPhase1 CXX C )

CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../../cmake/Modules)

FIND_PACKAGE(OpenGL REQUIRED)

SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTSVG TRUE)
SET(QT_USE_QTWEBKIT TRUE)

FIND_PACKAGE(Qt4 REQUIRED)

FIND_PACKAGE(PythonInterp REQUIRED)

include(ExternalProject)

SET(external_base ${CMAKE_CURRENT_BINARY_DIR}/base CACHE PATH "Path where external dependencies that are built will be installed (e.g., /usr/local or $HOME/FM_deps")

if (external_base STREQUAL "")
  message(SEND_ERROR "You must set the external_base variable to the root of the directory where external depenencies are to be installed, e.g., $HOME/FM_deps")
endif()

SET_PROPERTY(DIRECTORY PROPERTY EP_BASE ${external_base})
SET(external_install_dir "${external_base}/Install")

######################################################################
# PCRE Package
######################################################################

OPTION(FORCE_EXTERNAL_PCRE "Force use of external PCRE" OFF)

FIND_PACKAGE(PCRE)

if (FORCE_EXTERNAL_PCRE OR NOT PCRE_FOUND)
  MESSAGE("Will build PCRE from external sources")
  # Fetch PCRE

  set(cmake_args -DCMAKE_INSTALL_PREFIX:PATH=${external_install_dir} -DPCRE_SUPPORT_LIBBZ2=OFF -DPCRE_SUPPORT_LIBZ=OFF -DPCRE_SUPPORT_LIBEDIT=OFF -DPCRE_SUPPORT_LIBREADLINE=OFF)
  if (CMAKE_CROSSCOMPILING)
    set(cmake_args ${cmake_args} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
  endif ()

  ExternalProject_Add(
    PCRE-8-32
    URL http://sourceforge.net/projects/pcre/files/pcre/8.32/pcre-8.32.tar.gz/download
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Downloads
    SOURCE_DIR ${external_base}/PCRE
    CMAKE_ARGS ${cmake_args}
    )
endif ()

######################################################################
# FFTW Package
######################################################################

OPTION(FORCE_EXTERNAL_FFTW_FFTWF "Force use of external FFTW and FFTWF" OFF)

FIND_PACKAGE(FFTW)
FIND_PACKAGE(FFTWF)

if (FORCE_EXTERNAL_FFTW_FFTWF OR NOT FFTW_FOUND OR NOT FFTWF_FOUND)
  MESSAGE("Will build FFTW and FFTWF from external sources")
  # Fetch FFTW

  set(configure_args --prefix=${external_install_dir} --enable-threads --with-combined-threads --disable-fortran --disable-alloca)
  if(CMAKE_CROSSCOMPILING)
    set(configure_args ${configure_args} --host=${cross_host})
  endif(CMAKE_CROSSCOMPILING)
  
  ExternalProject_Add(
    FFTW-3-3-3-Fetch
    URL http://www.fftw.org/fftw-3.3.3.tar.gz
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Downloads
    SOURCE_DIR ${external_base}/FFTW
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    )

  ExternalProject_Add(
    FFTW-3-3-3-Build
    DOWNLOAD_COMMAND ""
    DEPENDS FFTW-3-3-3-Fetch
    SOURCE_DIR ${external_base}/FFTW
    BINARY_DIR ${external_base}/Build/FFTW-3-3-3
    CONFIGURE_COMMAND ${external_base}/FFTW/configure ${configure_args} 
    )

  ExternalProject_Add(
    FFTWF-3-3-3-Build
    DOWNLOAD_COMMAND ""
    DEPENDS FFTW-3-3-3-Fetch
    SOURCE_DIR ${external_base}/FFTW
    BINARY_DIR ${external_base}/Build/FFTWF-3-3-3
    CONFIGURE_COMMAND ${external_base}/FFTW/configure ${configure_args} --enable-single
    )
endif ()

######################################################################
# PORTAUDIO Package
######################################################################

OPTION(FORCE_EXTERNAL_PORTAUDIO "Force use of external PORTAUDIO" OFF)

FIND_PACKAGE(PORTAUDIO)

if (FORCE_EXTERNAL_PORTAUDIO OR NOT PORTAUDIO_FOUND)
  MESSAGE("Will build PORTAUDIO from external sources")
  # Fetch PORTAUDIO

  set(configure_args --prefix=${external_install_dir})
  if (CMAKE_CROSSCOMPILING)
    set(configure_args ${configure_args} --host=${cross_host})
  endif ()

  ExternalProject_Add(
    PORTAUDIO-V19-20111121
    URL http://www.portaudio.com/archives/pa_stable_v19_20111121.tgz
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Downloads
    SOURCE_DIR ${external_base}/PORTAUDIO
    CONFIGURE_COMMAND ${external_base}/PORTAUDIO/configure ${configure_args}
    )
endif ()


######################################################################
# ZLIB Package
######################################################################

OPTION(FORCE_EXTERNAL_ZLIB "Force use of external ZLIB" OFF)

FIND_PACKAGE(ZLIB)

if (FORCE_EXTERNAL_ZLIB OR NOT ZLIB_FOUND)
  MESSAGE("Will build ZLIB from external sources")
  # Fetch ZLIB

  set(cmake_args -DCMAKE_INSTALL_PREFIX:PATH=${external_install_dir})
  if (CMAKE_CROSSCOMPILING)
    set(cmake_args ${cmake_args} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
  endif ()

  ExternalProject_Add(
    ZLIB-1-2-7
    URL http://zlib.net/zlib-1.2.7.tar.gz
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Downloads
    SOURCE_DIR ${external_base}/ZLIB
    CMAKE_ARGS ${cmake_args}
    )
endif ()

######################################################################
# FFI Package
######################################################################

OPTION(FORCE_EXTERNAL_FFI "Force use of external FFI" OFF)

FIND_PACKAGE(FFI)

set(configure_args --prefix=${external_install_dir})
if(CMAKE_CROSSCOMPILING)
  set(configure_args ${configure_args} --host=${cross_host})
endif(CMAKE_CROSSCOMPILING)

if (FORCE_EXTERNAL_FFI OR NOT FFI_FOUND)
  MESSAGE("Will build FFI from external sources")
  # Fetch FFI
  ExternalProject_Add(
    FFI-3-0-13
    URL ftp://sourceware.org/pub/libffi/libffi-3.0.13.tar.gz
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Downloads
    SOURCE_DIR ${external_base}/FFI
    CONFIGURE_COMMAND ${external_base}/FFI/configure  ${configure_args}
    )
endif ()

######################################################################
# BLAS Package
######################################################################

OPTION(FORCE_EXTERNAL_BLAS "Force use of external BLAS" OFF)

FIND_PACKAGE(BLAS)

if (FORCE_EXTERNAL_BLAS OR NOT BLAS_FOUND)
  MESSAGE("Will build BLAS from external sources")

  SET(make_args libs BINARY=64)
  ExternalProject_Add(
    OpenBLAS-0-2-6
    URL https://github.com/xianyi/OpenBLAS/archive/v0.2.6.tar.gz
    SOURCE_DIR ${external_base}/OpenBLAS
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make ${make_args}
    INSTALL_COMMAND make "PREFIX=${external_install_dir}" install
    )  
endif()

######################################################################
# LAPACK Package
######################################################################

OPTION(FORCE_EXTERNAL_LAPACK "Force use of external LAPACK" OFF)

FIND_PACKAGE(LAPACK)

if (FORCE_EXTERNAL_LAPACK OR NOT LAPACK_FOUND)
  MESSAGE("Will build LAPACK from external sources")

  set(cmake_args -DCMAKE_INSTALL_PREFIX:PATH=${external_install_dir}
    -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF)
  if (CMAKE_CROSSCOMPILING)
    set(cmake_args ${cmake_args} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
  endif ()

  ExternalProject_Add(
    LAPACK
    URL http://www.netlib.org/lapack/lapack-3.4.2.tgz
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Downloads
    SOURCE_DIR ${external_base}/LAPACK
    CMAKE_ARGS ${cmake_args}
    )  
endif()


######################################################################
# ARPACK Package
######################################################################

OPTION(FORCE_EXTERNAL_ARPACK "Force use of external ARPACK" OFF)

FIND_PACKAGE(ARPACK)

if (FORCE_EXTERNAL_ARPACK OR NOT ARPACK_FOUND)
  MESSAGE("Will build ARPACK from external sources")

  set(configure_args --prefix=${external_install_dir})
  if (CMAKE_CROSSCOMPILING)
    # Not ideal
    set(configure_args ${configure_args} --host=${cross_host}
  --with-blas=${external_install_dir}/lib/libblas.a
  --with-lapack=${external_install_dir}/lib/liblapack.a
  --disable-shared --enable-static)
  endif ()

if (APPLE)
# Broken
  set (configure_args --disable-shared)
endif()

  if (NOT LAPACK_FOUND)
  ExternalProject_Add(
    ARPACK-NG-3-1-3
    URL http://forge.scilab.org/index.php/p/arpack-ng/downloads/get/arpack-ng-3.1.3.tar.gz
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Downloads
    PATCH_COMMAND patch -p1 ${external_base}/ARPACK-3-1-3/TESTS/Makefile.am ${CMAKE_SOURCE_DIR}/arpack.patch
    SOURCE_DIR ${external_base}/ARPACK-3-1-3
    DEPENDS LAPACK
    CONFIGURE_COMMAND ${external_base}/ARPACK-3-1-3/configure ${configure_args}
    )  
   else ()
  ExternalProject_Add(
    ARPACK-NG-3-1-3
    URL http://forge.scilab.org/index.php/p/arpack-ng/downloads/get/arpack-ng-3.1.3.tar.gz
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Downloads
    PATCH_COMMAND patch -p1 ${external_base}/ARPACK-3-1-3/TESTS/Makefile.am ${CMAKE_SOURCE_DIR}/arpack.patch
    SOURCE_DIR ${external_base}/ARPACK-3-1-3
    CONFIGURE_COMMAND ${external_base}/ARPACK-3-1-3/configure ${configure_args}
    )  
   endif ()

    ExternalProject_Add_Step(ARPACK-NG-3-1-3 automake
    COMMAND automake --add-missing
    WORKING_DIRECTORY ${external_base}/ARPACK-3-1-3
    DEPENDEES patch
    )


    ExternalProject_Add_Step(ARPACK-NG-3-1-3 autoreconf
    COMMAND autoreconf
    WORKING_DIRECTORY ${external_base}/ARPACK-3-1-3
    DEPENDEES automake
    DEPENDERS configure
    )

endif()


######################################################################
# LLVM+CLANG Package
######################################################################

OPTION(FORCE_EXTERNAL_LLVM_CLANG "Force use of external LLVM and CLANG" OFF)

if (NOT FORCE_EXTERNAL_LLVM_CLANG)
FIND_PACKAGE(LLVM)
FIND_PACKAGE(CLANG)
endif()

set(cmake_args -DCMAKE_INSTALL_PREFIX:PATH=${external_install_dir})
if (CMAKE_CROSSCOMPILING)
  set(cmake_args ${cmake_args} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
endif ()

if (FORCE_EXTERNAL_LLVM_CLANG OR NOT LLVM_FOUND OR NOT CLANG_FOUND)
  MESSAGE("Will build LLVM+CLANG from external sources")
  # Fetch LLVM
  ExternalProject_Add(
    LLVM-3-2-Fetch
    URL http://llvm.org/releases/3.2/llvm-3.2.src.tar.gz
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Downloads
    SOURCE_DIR ${external_base}/LLVM
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    )
  ExternalProject_Add(
    CLANG-3-2-Fetch
    URL http://llvm.org/releases/3.2/clang-3.2.src.tar.gz
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Downloads
    SOURCE_DIR ${external_base}/LLVM/tools/clang
    DEPENDS LLVM-3-2-Fetch
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    )
  ExternalProject_Add(
    LLVM-CLANG-Build
    DEPENDS CLANG-3-2-Fetch
    SOURCE_DIR ${external_base}/LLVM
    CMAKE_ARGS ${cmake_args}
    )    
endif ()

######################################################################
# SuiteSparse Package
######################################################################

OPTION(FORCE_EXTERNAL_SUITESPARSE "Force use of external SuiteSparse" OFF)

FIND_PACKAGE(SUITESPARSE)

if (FORCE_EXTERNAL_SUITESPARSE OR NOT SUITESPARSE_FOUND)
  MESSAGE("Will build SuiteSparse from external sources")
  SET(make_args library INSTALL_LIB=${external_install_dir}/lib INSTALL_INCLUDE=${external_install_dir}/include CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER})
  # Fetch SuiteSparse
  ExternalProject_Add(
    SuiteSparse-4-1-0
    URL http://www.cise.ufl.edu/research/sparse/SuiteSparse/SuiteSparse-4.2.1.tar.gz
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Downloads
    SOURCE_DIR ${external_base}/SuiteSparse
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make ${make_args}
    BUILD_IN_SOURCE 1    
    INSTALL_COMMAND make install ${make_args}
    )
endif ()

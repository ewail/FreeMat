######## CMake debugging macro
MACRO(WRITE_OUT_ALL_VARIABLES)
SET(DstFile ${CMAKE_CURRENT_BINARY_DIR}/AllVariables.txt)

#MESSAGE("DEBUG -- Writing out all variables to file ${DstFile}")

FILE(WRITE ${DstFile} "")
GET_CMAKE_PROPERTY(VARS VARIABLES)
FOREACH(var ${VARS})
FILE(APPEND ${DstFile} "${var} \"${${var}}\"\n")
ENDFOREACH(var ${VARS})
ENDMACRO(WRITE_OUT_ALL_VARIABLES)
### End Debugging tools


PROJECT(FreeMat CXX C )

ENABLE_LANGUAGE(Fortran)
INCLUDE(FortranCInterface)

SET(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} ${external_base}/Install)

SET(CPACK_PACKAGE_VERSION_MAJOR "4")
SET(CPACK_PACKAGE_VERSION_MINOR "2")
SET(CPACK_PACKAGE_VERSION_PATCH "")
SET(CPACK_PACKAGE_FILE_NAME "FreeMat-4.2")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "FreeMat 4.2")
SET(CPACK_PACKAGE_VERSION "4.2")
SET(VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")

CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

SET( RESOURCEDIR "${CMAKE_INSTALL_PREFIX}/share/FreeMat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
#SET( CMAKE_VERBOSE_MAKEFILE TRUE )
SET( LOCAL_PATH ${PROJECT_SOURCE_DIR}/dependencies/bin ${PROJECT_BINARY_DIR} /mingw/lib INTERNAL )

IF( MSVC )
	ADD_DEFINITIONS( /Dsnprintf=_snprintf /DNOMINMAX /DFREEMAT_VERSION="${VERSION}" /DRESOURCEDIR="${RESOURCEDIR}")
	#	ADD_DEFINITIONS( /D_HAS_ITERATOR_DEBUGGING=0 /D_SECURE_SCL=0 )
ELSE( MSVC )
	ADD_DEFINITIONS( -DFREEMAT_VERSION="${VERSION}" -DRESOURCEDIR="${RESOURCEDIR}")
	SET(CMAKE_SHARED_MODULE_LINK_FLAGS "${CMAKE_SHARED_MODULE_LINK_FLAGS} -export-dynamic")
	SET(CMAKE_SHARED_MODULE_LINK_CXX_FLAGS "${CMAKE_SHARED_MODULE_LINK_CXX_FLAGS} -export-dynamic")
ENDIF( MSVC )

IF( MINGW )
#    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -shared-libgcc -shared-libstdc++")
ENDIF(MINGW)

ADD_DEFINITIONS(-D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS)


SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTSVG TRUE)
SET(QT_USE_QTWEBKIT TRUE)

FIND_PACKAGE(Qt4 REQUIRED)

FIND_PACKAGE(PythonInterp REQUIRED)


ADD_DEFINITIONS( ${QT_DEFINITIONS} )
INCLUDE_DIRECTORIES( ${QT_INCLUDES} )

FIND_PACKAGE(OpenGL REQUIRED)

message("OpenGL include path is ${OPENGL_INCLUDE_DIR}")
if (APPLE)
# Broken
INCLUDE_DIRECTORIES("${OPENGL_INCLUDE_DIR}/Headers") 
else ()
INCLUDE_DIRECTORIES("${OPENGL_INCLUDE_DIR}/GL") 
endif ()

MACRO (QT4_GENERATE_UI infile outfile )
     GET_FILENAME_COMPONENT(infile ${infile} ABSOLUTE)

     ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
        COMMAND ${QT_UIC_EXECUTABLE}
        ARGS -o ${outfile} ${infile}
        DEPENDS ${infile})

     MACRO_ADD_FILE_DEPENDENCIES(${infile} ${outfile})
ENDMACRO (QT4_GENERATE_UI)

SET(OPTIONAL_LIBS "")

OPTION( FORCE_SYSTEM_LIBS "Only use system libraries. Don't build third party libs." )

######################################################################
# External Project Setup
######################################################################

include(ExternalProject)
SET(external_base ${CMAKE_CURRENT_BINARY_DIR}/deps/base CACHE PATH "Path where external dependencies that were built are installed")
SET(CMAKE_FIND_ROOT_PATH ${external_base}/Install ${CMAKE_FIND_ROOT_PATH})

######################################################################
# PCRE Package
######################################################################

FIND_PACKAGE(PCRE)

if (PCRE_FOUND)
  ADD_DEFINITIONS( -DHAVE_PCRE)
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${PCRE_LIBS})
  INCLUDE_DIRECTORIES(${PCRE_INCLUDE})
endif ()

######################################################################
# FFTW Package
######################################################################

FIND_PACKAGE(FFTW)

if (FFTW_FOUND)
  ADD_DEFINITIONS( -DHAVE_FFTW)
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${FFTW_LIBS})
  INCLUDE_DIRECTORIES(${FFTW_INCLUDE})
endif ()

######################################################################
# FFTWF Package
######################################################################

FIND_PACKAGE(FFTWF)

if (FFTWF_FOUND)
  ADD_DEFINITIONS( -DHAVE_FFTWF)
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${FFTWF_LIBS})
  INCLUDE_DIRECTORIES(${FFTWF_INCLUDE})
endif ()

######################################################################
# ZLIB Package
######################################################################

FIND_PACKAGE(ZLIB)

if (ZLIB_FOUND)
  ADD_DEFINITIONS( -DHAVE_ZLIB)
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${ZLIB_LIBS})
  INCLUDE_DIRECTORIES(${ZLIB_INCLUDE})
endif()


######################################################################
# ARPACK Package
######################################################################

FIND_PACKAGE(ARPACK)

if (ARPACK_FOUND)
  ADD_DEFINITIONS(-DHAVE_ARPACK)
# Ugly, ugly hack
# Should be  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${ARPACK_LIBS})
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${ARPACK_LIBS} lapack_c)
endif()


######################################################################
# FFI Package
######################################################################

if (APPLE)
  FIND_PACKAGE(FFI)
  if (FFI_FOUND)
    SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${FFI_LIBS})
    INCLUDE_DIRECTORIES(${FFI_INCLUDE})
  else ()
    message(FATAL_ERROR "FreeMat requires libFFI to compile")
  endif()
else()
  include(FindPkgConfig)
  message("Searching for libffi package (required)")
  pkg_check_modules(FFI REQUIRED libffi)
  ADD_DEFINITIONS(-DHAVE_FFI)
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${FFI_LDFLAGS})
  INCLUDE_DIRECTORIES(${FFI_INCLUDE_DIRS})
  message("Found libffi version ${FFI_VERSION} here ${FFI_LIBDIR}")
endif()

######################################################################
# SuiteSparse Package
######################################################################

FIND_PACKAGE(SUITESPARSE)

if (SuiteSparse_FOUND)
  ADD_DEFINITIONS(-DHAVE_SUITESPARSE)
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${SUITESPARSE_LIBS})
  INCLUDE_DIRECTORIES(${SUITESPARSE_INCLUDE})
endif ()


######################################################################
# PORTAUDIO Package
######################################################################

FIND_PACKAGE(PORTAUDIO)

if (PORTAUDIO_FOUND)
  ADD_DEFINITIONS(-DHAVE_PORTAUDIO19)
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${PORTAUDIO_LIBS})
  INCLUDE_DIRECTORIES(${PORTAUDIO_INCLUDE})
endif()

######################################################################
# VTK Support
######################################################################
OPTION(USE_VTK "Build with VTK support?" OFF)
IF (USE_VTK)
FIND_PACKAGE(VTK)
IF (VTK_FOUND)
  ADD_DEFINITIONS( -DHAVE_VTK )
  INCLUDE(${VTK_USE_FILE})
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} QVTK vtkFiltering vtkRendering vtkVolumeRendering vtkGraphics vtkIO vtkCommon vtkGeovis vtkImaging vtkInfovis vtkWidgets vtkHybrid vtkParallel)
ENDIF (VTK_FOUND)
ENDIF (USE_VTK)

######################################################################
# ITK Support
######################################################################
OPTION(USE_ITK "Build with InsightToolkit support?" OFF)
IF (USE_ITK)
FIND_PACKAGE(ITK)
IF (ITK_FOUND)
  ADD_DEFINITIONS( -DHAVE_ITK)
  INCLUDE(${ITK_USE_FILE})
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ITKBasicFilters)
ENDIF(ITK_FOUND)
ENDIF(USE_ITK)

######################################################################
# Boost Support
######################################################################
find_package( Boost COMPONENTS math_c99 )
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_definitions(-DHAVE_BOOST)
  set(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${Boost_LIBRARIES})
endif()

######################################################################
# LLVM Support
######################################################################
OPTION(USE_LLVM "Build with LLVM support?" ON)

FIND_PACKAGE(LLVM)
FIND_PACKAGE(CLANG)

IF (LLVM_FOUND AND CLANG_FOUND)
  add_definitions(-DHAVE_LLVM)
  include_directories(${LLVM_INCLUDE_DIRS})
  link_directories(${LLVM_LIBRARY_DIRS})
  llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES jit interpreter nativecodegen asmparser bitreader bitwriter codegen ipo linker selectiondag instrumentation)
  set(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${CLANG_LIBRARIES} ${REQ_LLVM_LIBRARIES})
ENDIF()

##############################################################################
#Extra libraries
IF(NOT WIN32)
  FIND_LIBRARY(NCURSES_LIBRARY NAMES ncurses DOC "Location of the ncurses library." PATH /mingw/lib)
  SET(EXTRA_LIBRARIES ${NCURSES_LIBRARY}  )
ENDIF(NOT WIN32)

INCLUDE_DIRECTORIES( libs/libFreeMat libs/libGraphics
libs/libMex libs/libXP libs/libCore libs/libFN libs/libMatC
libs/libMath/libDynBlas ${QT_LIBRARIES} src toolbox/jit)


##############################################################################
# BLAS is always linked dynamically
ADD_DEFINITIONS( -DDYN_BLAS )


if( MSVC )
  add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS )
  add_definitions( -D_SCL_SECURE_NO_WARNINGS -DCRT_NONSTDC_NO_WARNINGS )
  add_definitions( -D_SCL_SECURE_NO_DEPRECATE )
  #add_definitions( -wd4146 -wd4503 -wd4996 -wd4800 -wd4244 -wd4624 )
  #add_definitions( -wd4355 -wd4715 )
endif( MSVC )

ADD_SUBDIRECTORY( libs )
ADD_SUBDIRECTORY( src )

####################################################################################
#Install section
#IF( WIN32 )
#	FILE(GLOB INST_DLL "dependencies/bin/*.dll" )
#	INSTALL(FILES ${INST_DLL} DESTINATION bin )
#ENDIF( WIN32 )
#
####################################################################################
#Setup section

INCLUDE(InstallRequiredSystemLibraries)

GET_TARGET_PROPERTY(FreeMat_Loc FreeMat LOCATION)
# Do not run helpgen automatically -- there are too many things that
# have to be checked after it is run.
#INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${FreeMat_Loc} -f run_helpgen WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})" )
#INSTALL(DIRECTORY toolbox DESTINATION share/FreeMat PATTERN "*.m" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
#INSTALL(DIRECTORY help DESTINATION share/FreeMat PATTERN "*" PERMISSIONS WORLD_READ )

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FreeMat")

SET(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/COPYING)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FreeMat is a free environment for rapid engineering and scientific prototyping and data processing.")

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "FreeMat")

IF(UNIX AND NOT WIN32 AND NOT APPLE)
  SET(INST_DIR "share/FreeMat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
  INSTALL(DIRECTORY toolbox DESTINATION ${INST_DIR} PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(DIRECTORY tests DESTINATION ${INST_DIR}/toolbox PATTERN "*" PATTERN ".svn" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(DIRECTORY help/html DESTINATION "${INST_DIR}/help" PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(DIRECTORY help/text DESTINATION "${INST_DIR}/help" PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(FILES "help/latex/FreeMat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.pdf" DESTINATION "${INST_DIR}/help/pdf" )
ENDIF(UNIX AND NOT WIN32 AND NOT APPLE)

IF(WIN32)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\FreeMat.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "FreeMat")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\freemat.sf.net")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\freemat.sf.net")
  #SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
  #SET(CPACK_NSIS_MODIFY_PATH ON)

  INSTALL(FILES
	${QT_LIBRARY_DIR}/QtCore4.dll
	${QT_LIBRARY_DIR}/QtGui4.dll
	${QT_LIBRARY_DIR}/QtNetwork4.dll
	${QT_LIBRARY_DIR}/QtOpenGL4.dll
	${QT_LIBRARY_DIR}/QtXml4.dll
	${QT_LIBRARY_DIR}/QtSvg4.dll
	${QT_LIBRARY_DIR}/QtWebKit4.dll
	DESTINATION bin 
	PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
	COMPONENT Runtime)

  INSTALL(FILES
# Hackish hack because of current discovery problems.
	${QT_LIBRARY_DIR}/../lib/qt4/plugins/imageformats/qjpeg4.dll
	${QT_LIBRARY_DIR}/../lib/qt4/plugins/imageformats/qgif4.dll
	${QT_LIBRARY_DIR}/../lib/qt4/plugins/imageformats/qsvg4.dll
	${QT_LIBRARY_DIR}/../lib/qt4/plugins/imageformats/qtiff4.dll
	DESTINATION bin/imageformats 
	PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
	COMPONENT Runtime)

# PERMISSIONS WORLD_EXECUTE

  INSTALL(DIRECTORY toolbox DESTINATION . COMPONENT Toolbox PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(DIRECTORY tests DESTINATION toolbox COMPONENT tests PATTERN "*" PATTERN ".svn" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(DIRECTORY help/html DESTINATION help COMPONENT help PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  #INSTALL(DIRECTORY help/text DESTINATION help PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(FILES "help/latex/FreeMat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.pdf" DESTINATION help/pdf COMPONENT help)
ENDIF(WIN32)

INSTALL(FILES ${RUNTIME_LIBS}
  DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE  )

SET(CPACK_PACKAGE_EXECUTABLES "FreeMat" "FreeMat")

#TODO Handle gfortran vs g77 issue: libgfortran vs libg2c

#add_custom_target( helpgen
#	COMMENT "Building help files"
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/helpgen.py ${CMAKE_SOURCE_DIR}
#	COMMAND ${FreeMat_Loc} "-nogui" "-p" ${CMAKE_SOURCE_DIR} "-f" "fraggen ${CMAKE_SOURCE_DIR}/help/tmp"
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/mergefragments.py ${CMAKE_SOURCE_DIR}/help
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/mergemfiles.py ${CMAKE_SOURCE_DIR}/src
#	VERBATIM
#	)

add_custom_target( fragdb
  COMMENT "Building fragment database"
  COMMAND ${FreeMat_Loc} "-nogui" "-r" ${CMAKE_SOURCE_DIR} "-f" "fraggen ${CMAKE_SOURCE_DIR}/doc/fragments"
  VERBATIM
  )

#add_custom_target( helpgen_fast
#	COMMENT "Building help files (no fragment generation)"
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/helpgen.py ${CMAKE_SOURCE_DIR}
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/mergefragments.py ${CMAKE_SOURCE_DIR}/help
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/mergemfiles.py ${CMAKE_SOURCE_DIR}/src
#	VERBATIM
#	)

#add_custom_target( helpgen_only
#	COMMENT "Building help files"
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/helpgen.py ${CMAKE_SOURCE_DIR}
#	VERBATIM
#	)

add_custom_target( mergem
	COMMENT "Merging M files"
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/mergemfiles.py ${CMAKE_SOURCE_DIR}/src
	VERBATIM
	)

add_custom_target( jitgen
        COMMENT "Building list of scalar JIT functions"
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/jitgen.py ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/toolbox/jit/CScalarFuncs.hpp
	VERBATIM
	)


WRITE_OUT_ALL_VARIABLES()

ENABLE_TESTING()
add_subdirectory(tests)

find_package(Doxygen)

if (DOXYGEN_FOUND)
add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR/doc/Doxyfile} 
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
  COMMENT "Generating documentation"
)
endif(DOXYGEN_FOUND)

add_custom_target(index
  COMMENT "Building index"
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/makeindex.py ${CMAKE_SOURCE_DIR}/help/xml/index.xml ${CMAKE_SOURCE_DIR}/help/html/modules.txt
  DEPENDS doc
  VERBATIM
)

add_custom_target(latexdoc
  COMMENT "LaTeX Documentation"
  COMMAND make
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/help/latex
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/help/latex/refman.pdf ${CMAKE_CURRENT_SOURCE_DIR}/help/latex/FreeMat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.pdf
  DEPENDS doc
)

add_custom_target(textdoc
  COMMENT "Text generation"
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/help/maketxt.sh"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/help
  DEPENDS doc
)

add_custom_target(fulldoc
  COMMENT "Build full HTML documentation, LaTeX and Text versions"
  DEPENDS doc index latexdoc textdoc
)

add_custom_target(packhelp
  COMMENT "Package documentation for commit to SVN"
  DEPENDS fulldoc
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/help/packhelp.sh"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/help
)
  

INCLUDE(CPack)

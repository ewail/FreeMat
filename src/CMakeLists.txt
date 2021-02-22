INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}  ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_BINARY_DIR}/../libs/libXP")

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/application.hpp ${CMAKE_CURRENT_BINARY_DIR}/application.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/FuncMode.hpp ${CMAKE_CURRENT_BINARY_DIR}/FuncMode.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/ScriptMode.hpp ${CMAKE_CURRENT_BINARY_DIR}/ScriptMode.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/MainApp.hpp ${CMAKE_CURRENT_BINARY_DIR}/MainApp.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/Terminal.hpp ${CMAKE_CURRENT_BINARY_DIR}/Terminal.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/DumbTerminal.hpp ${CMAKE_CURRENT_BINARY_DIR}/DumbTerminal.moc.cpp )

QT4_ADD_RESOURCES( FreeMat_qrc ${CMAKE_CURRENT_SOURCE_DIR}/FreeMat.qrc  )

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Loader.cpp
	DEPENDS MainApp.cpp
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/tools/siggen.py ${CMAKE_CURRENT_SOURCE_DIR} Main ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating function signatures"
	VERBATIM
	)

ADD_EXECUTABLE( FreeMat MACOSX_BUNDLE WIN32
${CMAKE_CURRENT_BINARY_DIR}/application.moc.cpp
application.cpp
${CMAKE_CURRENT_BINARY_DIR}/FuncMode.moc.cpp
${CMAKE_CURRENT_BINARY_DIR}/ScriptMode.moc.cpp
FuncMode.cpp
ScriptMode.cpp
FuncTerminal.cpp
${CMAKE_CURRENT_BINARY_DIR}/MainApp.moc.cpp
MainApp.cpp
main.cpp
${CMAKE_CURRENT_BINARY_DIR}/DumbTerminal.moc.cpp
DumbTerminal.cpp
${CMAKE_CURRENT_BINARY_DIR}/Terminal.moc.cpp
${CMAKE_CURRENT_BINARY_DIR}/Loader.cpp
Terminal.cpp
${FreeMat_qrc}
freemat.rc
# Added per: http://www.cmake.org/pipermail/cmake/2010-July/038172.html
dummy.f
)

#${X86TARGET_OBJ}

#IF( MSVC )
#	SET( LAPACK_LIBRARY ${lapack} )
#ENDIF( MSVC )

IF( USE_DYNLINK )
	TARGET_LINK_LIBRARIES( FreeMat blas )
ENDIF( USE_DYNLINK )


if (USE_VTK)
set (VTK_WRAPPER_LIBS
vtkCommonWrapper        vtkIOWrapper
vtkFilteringWrapper  vtkParallelWrapper
vtkGeovisWrapper        vtkRenderingWrapper
vtkGraphicsWrapper    vtkViewsWrapper
vtkHybridWrapper        vtkVolumeRenderingWrapper
vtkImagingWrapper      vtkWidgetsWrapper
vtkInfovisWrapper )
endif (USE_VTK)

if (USE_ITK)
set (ITK_WRAPPER_LIBS itkWrapper)
endif (USE_ITK)

TARGET_LINK_LIBRARIES( FreeMat
Core
FN
${VTK_WRAPPER_LIBS}
${ITK_WRAPPER_LIBS}
Graphics
FreeMatlib
XP
Mex
MatC
levmar
lapack_c
dynblas
blasref
${QT_QTCORE_LIBRARY}
${QT_QTGUI_LIBRARY}
${QT_QTNETWORK_LIBRARY}
${QT_QTOPENGL_LIBRARY}
${QT_QTXML_LIBRARY}
${QT_QTSVG_LIBRARY}
${QT_QTMAIN_LIBRARY}
${QT_QTWEBKIT_LIBRARY}
${OPENGL_LIBRARIES}
${EXTRA_LIBRARIES}
${OPTIONAL_LIBS}
)


#IF(MSVC)
#  SET_TARGET_PROPERTIES(FreeMat PROPERTIES LINK_FLAGS "/INCLUDE:_X86TargetMachineModule")
#ENDIF(MSVC)

#IF( MSVC )
#	set_target_properties( FreeMat PROPERTIES LINK_FLAGS "/MANIFEST:NO  /ALLOWISOLATION:NO" )
#ENDIF( MSVC )

INSTALL(TARGETS FreeMat BUNDLE DESTINATION . COMPONENT Runtime
		RUNTIME DESTINATION bin COMPONENT Runtime)

# Mac OS X specific packaging info
IF(APPLE)
SET(MACOSX_BUNDLE_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/appIcon.icns)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/appIcon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

#  SET(CPACK_BUNDLE_PLIST ${PROJECT_SOURCE_DIR}/tools/Info.plist )
#  SET(CPACK_BUNDLE_STARTUP_COMMAND ${PROJECT_BINARY_DIR}/src/FreeMat )
#  SET(CPACK_BUNDLE_NAME "FreeMat")
#  SET(CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/src/appIcon.icns)

SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/FreeMat")
IF(APPLE)
  SET(plugin_dest_dir FreeMat.app/Contents)
  SET(qtconf_dest_dir FreeMat.app/Contents/Resources)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/FreeMat.app")
ENDIF(APPLE)
IF(WIN32)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/FreeMat.exe")
ENDIF(WIN32)


#--------------------------------------------------------------------------------
# Install needed Qt plugins by copying directories from the qt installation
# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)

#--------------------------------------------------------------------------------
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
INSTALL(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime)


#--------------------------------------------------------------------------------
# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.

# directories to look for dependencies
SET(DIRS ${QT_LIBRARY_DIRS})

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ escaped
# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
# Note that the image plugins depend on QtSvg and QtXml, and it got those copied
# over.
INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime)


# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well
set(CPACK_BINARY_DRAGNDROP ON)

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/toolbox DESTINATION ${qtconf_dest_dir} PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/tests DESTINATION ${qtconf_dest_dir}/toolbox PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/help/html DESTINATION ${qtconf_dest_dir}/help PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/help/text DESTINATION ${qtconf_dest_dir}/help PATTERN "*" PATTERN  ".svn" EXCLUDE PERMISSIONS WORLD_READ )
INSTALL(FILES
"${CMAKE_SOURCE_DIR}/help/latex/FreeMat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.pdf"
 DESTINATION "${qtconf_dest_dir}/help/pdf" )
INSTALL(FILES
"${CMAKE_SOURCE_DIR}/help/latex/FreeMat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.pdf"
 DESTINATION ".")

#INSTALL(DIRECTORY /opt/local/lib/Resources/qt_menu.nib DESTINATION ${qtconf_dest_dir} COMPONENT Runtime)

ENDIF(APPLE)

if (APPLE)
INSTALL( FILES ${PROJECT_SOURCE_DIR}/tools/blas.ini DESTINATION FreeMat.app/Contents/MacOS)
else()
INSTALL( FILES ${PROJECT_SOURCE_DIR}/tools/blas.ini DESTINATION bin )
endif()


#Ideally, these would be found automatically
IF(WIN32 AND CMAKE_CROSSCOMPILING)
message("Cross compiling and installing")
FIND_LIBRARY(INST_PCRE_LIB NAMES libpcre-1.dll)
FIND_LIBRARY(INST_FFI_LIB NAMES libffi-6.dll)
FIND_LIBRARY(INST_GFORTRAN_LIB NAMES libgfortran-3.dll)
FIND_LIBRARY(INST_PORTAUDIO_LIB NAMES libportaudio-2.dll)
FIND_LIBRARY(INST_QUADMATH_LIB NAMES libquadmath-0.dll)
FIND_LIBRARY(INST_JPEG_LIB NAMES libjpeg-62.dll)
FIND_LIBRARY(INST_TIFF_LIB NAMES libtiff-3.dll)
INSTALL(FILES ${INST_PCRE_LIB} DESTINATION bin COMPONENT Runtime)
INSTALL(FILES ${INST_FFI_LIB} DESTINATION bin COMPONENT Runtime)
INSTALL(FILES ${INST_GFORTRAN_LIB} DESTINATION bin COMPONENT Runtime)
INSTALL(FILES ${INST_PORTAUDIO_LIB} DESTINATION bin COMPONENT Runtime)
INSTALL(FILES ${INST_QUADMATH_LIB} DESTINATION bin COMPONENT Runtime)
INSTALL(FILES ${INST_JPEG_LIB} DESTINATION bin COMPONENT Runtime)
INSTALL(FILES ${INST_TIFF_LIB} DESTINATION bin COMPONENT Runtime)
ENDIF()


WRITE_OUT_ALL_VARIABLES()

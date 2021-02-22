INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTSVG_INCLUDE_DIR} )

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.hpp ${CMAKE_CURRENT_BINARY_DIR}/HandleUIControl.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.hpp ${CMAKE_CURRENT_BINARY_DIR}/HandleMultiEdit.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.hpp ${CMAKE_CURRENT_BINARY_DIR}/HandleWindow.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.hpp ${CMAKE_CURRENT_BINARY_DIR}/GLWindow.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/VTKWindow.hpp ${CMAKE_CURRENT_BINARY_DIR}/VTKWindow.moc.cpp )


if (USE_VTK)

ADD_EXECUTABLE (vtkWrapFreeMat
${CMAKE_SOURCE_DIR}/tools/vtkwrap/vtkWrapFreeMat.c
${CMAKE_SOURCE_DIR}/tools/vtkwrap/vtkParse.tab.c)

ADD_SUBDIRECTORY(vtk)

endif(USE_VTK)

if (USE_ITK)
ADD_SUBDIRECTORY(itk)
endif(USE_ITK)

set (VTK_WRAPPER_LIBS 
vtkCommonWrapper        vtkIOWrapper
vtkFilteringWrapper  vtkParallelWrapper
vtkGeovisWrapper        vtkRenderingWrapper
vtkGraphicsWrapper    vtkViewsWrapper
vtkHybridWrapper        vtkVolumeRenderingWrapper
vtkImagingWrapper      vtkWidgetsWrapper
vtkInfovisWrapper)


set( GRAPHICS_SOURCES
GLRenderEngine.cpp 
HandleAxis.cpp 
HandleCommands.cpp 
HandleContour.cpp 
HandleFigure.cpp 
HandleImage.cpp 
HandleLineSeries.cpp 
HandleObject.cpp 
HandleProperty.cpp 
HandleSurface.cpp 
HandlePatch.cpp
HandleText.cpp 
HandleWindow.cpp 
QTRenderEngine.cpp 
RenderEngine.cpp 
HandleUIControl.cpp 
HandleMultiEdit.cpp 
GLWindow.cpp
trackball.cpp
VTKWindow.cpp
VTKWrap.cpp
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Loader.cpp
	DEPENDS ${GRAPHICS_SOURCES}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/tools/siggen.py
	${CMAKE_CURRENT_SOURCE_DIR} Graphics ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating function signatures"
	VERBATIM
	)


ADD_LIBRARY( Graphics 
${GRAPHICS_SOURCES}
${CMAKE_CURRENT_BINARY_DIR}/Loader.cpp
${CMAKE_CURRENT_BINARY_DIR}/VTKWindow.moc.cpp
${CMAKE_CURRENT_BINARY_DIR}/HandleUIControl.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/HandleMultiEdit.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/HandleWindow.moc.cpp
${CMAKE_CURRENT_BINARY_DIR}/GLWindow.moc.cpp
)



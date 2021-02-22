set(wrapcmd "vtkWrapFreeMat")

INCLUDE(${VTK_KITS_DIR}/vtkWidgetsKit.cmake)

foreach(class ${VTK_WIDGETS_CLASSES})
  if (NOT VTK_CLASS_WRAP_EXCLUDE_${class})
    set(src_name "${VTK_WIDGETS_HEADER_DIR}/${class}.h")
    set(dest_name "${CMAKE_CURRENT_SOURCE_DIR}/${class}.cpp")
    set(VTK_WIDGETS_SRC ${VTK_WIDGETS_SRC} ${dest_name})
    add_custom_command(
      OUTPUT ${dest_name}
      DEPENDS ${wrapcmd}
      COMMAND ${wrapcmd}
      ARGS ${src_name} ${CMAKE_SOURCE_DIR}/tools/vtkwrap/hints 1 ${dest_name} VTKWidgets
      COMMENT "Generating ${dest_name} from ${src_name}"
      VERBATIM)
  endif (NOT VTK_CLASS_WRAP_EXCLUDE_${class})
endforeach(class)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Loader.cpp
	DEPENDS ${VTK_WIDGETS_SRC}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/tools/siggen.py ${CMAKE_CURRENT_SOURCE_DIR} VTKWidgets
	COMMENT "Generating function signatures for ${CMAKE_CURRENT_SOURCE_DIR} with name Widgets"
	VERBATIM
	)

ADD_LIBRARY(vtkWidgetsWrapper
${VTK_WIDGETS_SRC}
${CMAKE_CURRENT_SOURCE_DIR}/Loader.cpp
)

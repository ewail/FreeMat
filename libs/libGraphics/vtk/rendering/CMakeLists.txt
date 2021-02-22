set(wrapcmd "vtkWrapFreeMat")

INCLUDE(${VTK_KITS_DIR}/vtkRenderingKit.cmake)

foreach(class ${VTK_RENDERING_CLASSES})
  if (NOT VTK_CLASS_WRAP_EXCLUDE_${class})
    set(src_name "${VTK_RENDERING_HEADER_DIR}/${class}.h")
    set(dest_name "${CMAKE_CURRENT_SOURCE_DIR}/${class}.cpp")
    set(VTK_RENDERING_SRC ${VTK_RENDERING_SRC} ${dest_name})
    add_custom_command(
      OUTPUT ${dest_name}
      DEPENDS ${wrapcmd}
      COMMAND ${wrapcmd}
      ARGS ${src_name} ${CMAKE_SOURCE_DIR}/tools/vtkwrap/hints 1 ${dest_name} VTKRendering
      COMMENT "Generating ${dest_name} from ${src_name}"
      VERBATIM)
  endif (NOT VTK_CLASS_WRAP_EXCLUDE_${class})
endforeach(class)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Loader.cpp
	DEPENDS ${VTK_RENDERING_SRC}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/tools/siggen.py ${CMAKE_CURRENT_SOURCE_DIR} VTKRendering
	COMMENT "Generating function signatures for ${CMAKE_CURRENT_SOURCE_DIR} with name Rendering"
	VERBATIM
	)

ADD_LIBRARY(vtkRenderingWrapper
${VTK_RENDERING_SRC}
${CMAKE_CURRENT_SOURCE_DIR}/Loader.cpp
)

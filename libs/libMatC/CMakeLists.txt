
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )

ADD_LIBRARY( MatC 
	JITFactory.cpp
	CJitFunc.cpp
	CJitFuncClang.cpp
	CArray.cpp
)

# Uncommenting for now... needs work
#ADD_DEPENDENCIES(MatC jitgen)

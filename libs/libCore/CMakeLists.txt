
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}  ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/helpwidget.hpp ${CMAKE_CURRENT_BINARY_DIR}/helpwidget.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/XML.hpp ${CMAKE_CURRENT_BINARY_DIR}/XML.moc.cpp )

set(CORE_SOURCES
Abs.cpp 
Binary.cpp 
Ceil.cpp 
Conj.cpp 
CumProd.cpp 
CumSum.cpp 
Dot.cpp
Floor.cpp 
Max.cpp 
Min.cpp 
Prod.cpp 
RCond.cpp 
Round.cpp 
Sum.cpp 
Time.cpp 
Variance.cpp 
RealImag.cpp 
Sort.cpp 
Unique.cpp 
XNorm.cpp 
Analyze.cpp 
Cast.cpp 
Constants.cpp 
Reshape.cpp 
Zeros.cpp 
Ones.cpp 
Cell.cpp 
Struct.cpp 
Directory.cpp 
FFT.cpp 
helpwidget.cpp 
Help.cpp 
Hex.cpp 
Clear.cpp 
Exist.cpp 
Find.cpp 
Is.cpp 
Size.cpp 
Path.cpp 
Inspect.cpp 
Audio.cpp 
Random.cpp 
RanLib.cpp 
Image.cpp 
File.cpp 
Printf.cpp 
IO.cpp 
Conv.cpp 
Eval.cpp 
Debug.cpp 
LU.cpp 
QR.cpp 
Eigen.cpp 
SVD.cpp 
Array.cpp 
Version.cpp 
Error.cpp 
Misc.cpp 
Mean.cpp 
StringOps.cpp 
Transcendental.cpp 
Trigonometric.cpp 
Utils.cpp 
MatIO.cpp 
XML.cpp 
Cstruct.cpp 
Control.cpp 
Sparse.cpp 
Matrix.cpp )



add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Loader.cpp
	DEPENDS ${CORE_SOURCES}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/tools/siggen.py
	${CMAKE_CURRENT_SOURCE_DIR} Core ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating function signatures"
	VERBATIM
	)

ADD_LIBRARY( Core 
${CORE_SOURCES}
${CMAKE_CURRENT_BINARY_DIR}/Loader.cpp
${CMAKE_CURRENT_BINARY_DIR}/helpwidget.moc.cpp
${CMAKE_CURRENT_BINARY_DIR}/XML.moc.cpp )

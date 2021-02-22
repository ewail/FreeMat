set(ITK_SRC
AdaptiveHistogramEqualization.cpp
BinomialBlur.cpp
BlackTopHat.cpp
BSplineUpsample.cpp
Canny.cpp
ConnectedComponents.cpp
ConnectedThreshold.cpp
CurvatureAnisotropicDiffusion.cpp
Derivative.cpp
Dilate.cpp
Erode.cpp
GradientAnisotropicDiffusion.cpp
GrayscaleDilate.cpp
GrayscaleErode.cpp
GrayscaleFillhole.cpp
GrayscaleGrindPeak.cpp
GrayscaleMorphologicalClosing.cpp
GrayscaleMorphologicalOpening.cpp
HardConnectedComponents.cpp
HConcave.cpp
HConvex.cpp
HMaxima.cpp
HMinima.cpp
Laplacian.cpp
LaplacianRecursiveGaussian.cpp
LaplacianSharpening.cpp
Mean.cpp
Median.cpp
MorphologicalGradient.cpp
Noise.cpp
NeighborhoodConnected.cpp
RecursiveGaussian.cpp
RelabelComponents.cpp
SmoothingRecursiveGaussian.cpp
SimpleContourExtractor.cpp
Sobel.cpp
ThresholdLabeler.cpp
WhiteTopHat.cpp
)

set(CMAKE_CXX_FLAGS "-fpermissive")
#set_source_files_properties(AdaptiveHistogramEqualization.cpp PROPERTIES COMPILE_FLAGS -fpermissive)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Loader.cpp
	DEPENDS ${ITK_SRC}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/tools/siggen.py ${CMAKE_CURRENT_SOURCE_DIR} ITK
	COMMENT "Generating function signatures for ${CMAKE_CURRENT_SOURCE_DIR} with name ITK"
	VERBATIM
	)

ADD_LIBRARY(itkWrapper
${ITK_SRC}
${CMAKE_CURRENT_SOURCE_DIR}/Loader.cpp
)

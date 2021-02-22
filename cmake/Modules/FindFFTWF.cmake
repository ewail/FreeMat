######################################################################
# FFTWF Package
######################################################################

MESSAGE("Searching for system FFTWF package")
FIND_LIBRARY(FFTWF_LIBRARY NAMES fftw3f fftw3f-3 libfftw3f-3 DOC "Location of the FFTWF library")
FIND_PATH(FFTWF_INCLUDE_DIR NAMES fftw3.h DOC "Location of fftw3.h")
if (FFTWF_LIBRARY AND FFTWF_INCLUDE_DIR)
  SET(FFTWF_FOUND TRUE)
  SET(FFTWF_INCLUDE ${FFTWF_INCLUDE_DIR})
  SET(FFTWF_LIBS ${FFTWF_LIBRARY})
  MESSAGE("Found FFTWF here ${FFTWFo_LIBRARY}")
else ()
  SET(FFTWF_FOUND FALSE)
  MESSAGE("Unable to find FFTWF")
endif ()


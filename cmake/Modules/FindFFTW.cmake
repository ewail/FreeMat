######################################################################
# FFTW Package
######################################################################

MESSAGE("Searching for system FFTW package")
FIND_LIBRARY(FFTW_LIBRARY NAMES fftw3 fftw3-3 libfftw3-3 DOC "Location of the FFTW library")
FIND_PATH(FFTW_INCLUDE_DIR NAMES fftw3.h DOC "Location of fftw3.h")
if (FFTW_LIBRARY AND FFTW_INCLUDE_DIR)
  SET(FFTW_FOUND TRUE)
  SET(FFTW_INCLUDE ${FFTW_INCLUDE_DIR})
  SET(FFTW_LIBS ${FFTW_LIBRARY})
  MESSAGE("Found FFTW here ${FFTW_LIBRARY}")
else ()
  SET(FFTW_FOUND FALSE)
  MESSAGE("Unable to find FFTW")
endif ()


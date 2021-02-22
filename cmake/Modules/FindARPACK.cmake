######################################################################
# ARPACK Package
######################################################################

MESSAGE("Searching for system ARPACK package")
FIND_LIBRARY(ARPACK_LIBRARY NAMES arpack DOC "Location of the ARPACK library" PATHS ${LOCAL_PATH})
if (ARPACK_LIBRARY)
  SET(ARPACK_FOUND TRUE)
  SET(ARPACK_LIBS ${ARPACK_LIBRARY})
  MESSAGE("Found ARPACK here ${ARPACK_LIBRARY}")
else ()
  SET(ARPACK_FOUND FALSE)
  MESSAGE("Unable to find ARPACK")
endif ()

######################################################################
# ZLIB Package
######################################################################

MESSAGE("Searching for system ZLIB package")
FIND_LIBRARY(ZLIB_LIBRARY NAMES zlib z DOC "Location of the ZLIB library" PATHS ${LOCAL_PATH})
FIND_PATH(ZLIB_INCLUDE_DIR NAMES zlib.h DOC "Location of zlib3.h" PATHS ${LOCAL_PATH})
if (ZLIB_LIBRARY AND ZLIB_INCLUDE_DIR)
  SET(ZLIB_FOUND TRUE)
  SET(ZLIB_INCLUDE ${ZLIB_INCLUDE_DIR})
  SET(ZLIB_LIBS ${ZLIB_LIBRARY})
  MESSAGE("Found ZLIB here ${ZLIB_LIBRARY}")
else ()
  SET(ZLIB_FOUND FALSE)
  MESSAGE("Unable to find ZLIB")
endif ()


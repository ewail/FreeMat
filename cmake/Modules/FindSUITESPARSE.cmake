######################################################################
# SUITESPARSE Package
######################################################################

MESSAGE("Searching for system SUITESPARSE package")
FIND_LIBRARY(AMD_LIBRARY NAMES amd DOC "Location of the AMD library (from SuiteSparse)" PATHS ${LOCAL_PATH})
FIND_PATH(AMD_INCLUDE_DIR NAMES amd.h DOC "Location of amd.h" PATHS ${LOCAL_PATH} /usr/include/ufsparse /usr/include/suitesparse)
FIND_LIBRARY(UMFPACK_LIBRARY NAMES umfpack DOC "Location of the UMFPACK library (from SuiteSparse)" PATHS ${LOCAL_PATH})
FIND_PATH(UMFPACK_INCLUDE_DIR NAMES umfpack.h DOC "Location of umfpack.h" PATHS ${LOCAL_PATH} /usr/include/ufsparse /usr/include/suitesparse)
if (AMD_LIBRARY AND AMD_INCLUDE_DIR AND UMFPACK_LIBRARY AND UMFPACK_INCLUDE_DIR)
  SET(SUITESPARSE_FOUND TRUE)
  SET(SUITESPARSE_INCLUDE ${AMD_INCLUDE_DIR} ${UMFPACK_INCLUDE_DIR})
  SET(SUITESPARSE_LIBS ${AMD_LIBRARY} ${UMFPACK_LIBRARY})
  MESSAGE("Found SUITESPARSE here ${SUITESPARSE_LIBS}")
else ()
  SET(SUITESPARSE_FOUND FALSE)
  MESSAGE("Unable to find SUITESPARSE")
endif ()

######################################################################
# PCRE Package
######################################################################

MESSAGE("Searching for system PCRE package")
FIND_LIBRARY(PCRE_LIBRARY NAMES pcre DOC "Location of the PCRE library" PATHS ${LOCAL_PATH})
FIND_PATH(PCRE_INCLUDE_DIR NAMES pcre.h DOC "Location of pcre.h" PATHS ${LOCAL_PATH})
if (PCRE_LIBRARY AND PCRE_INCLUDE_DIR)
  SET(PCRE_FOUND TRUE)
  SET(PCRE_INCLUDE ${PCRE_INCLUDE_DIR})
  SET(PCRE_LIBS ${PCRE_LIBRARY})
  MESSAGE("Found PCRE here ${PCRE_LIBRARY}")
else ()
  SET(PCRE_FOUND FALSE)
  MESSAGE("Unable to find PCRE")
endif ()


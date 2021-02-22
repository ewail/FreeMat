######################################################################
# FFI Package
######################################################################

MESSAGE("Searching for system FFI package")
FIND_LIBRARY(FFI_LIBRARY NAMES ffi DOC "Location of the FFI library" PATHS ${LOCAL_PATH})
FIND_PATH(FFI_INCLUDE_DIR NAMES ffi.h DOC "Location of ffi3.h" PATHS ${LOCAL_PATH} /usr/include/ffi /usr/lib64/libffi-3.0.9/include /usr/lib64/libffi-3.0.10/include)
if (FFI_LIBRARY AND FFI_INCLUDE_DIR)
  SET(FFI_FOUND TRUE)
  SET(FFI_INCLUDE ${FFI_INCLUDE_DIR})
  SET(FFI_LIBS ${FFI_LIBRARY})
  MESSAGE("Found FFI here ${FFI_LIBRARY}")
else ()
  SET(FFI_FOUND FALSE)
  MESSAGE("Unable to find FFI")
endif ()


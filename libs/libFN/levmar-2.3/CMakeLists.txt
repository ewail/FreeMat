# Modified CML for inclusion in FreeMat code base
# levmar CMake file; see http://www.cmake.org and 
#                        http://www.insightsoftwareconsortium.org/wiki/index.php/CMake_Tutorial

# compiler flags
REMOVE_DEFINITIONS(-DLINSOLVERS_RETAIN_MEMORY)

# levmar library source files
ADD_LIBRARY(levmar STATIC
  lm.c Axb.c misc.c lmlec.c lmbc.c lmblec.c
  lm.h misc.h compiler.h
)

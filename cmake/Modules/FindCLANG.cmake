######################################################################
# CLANG Package
######################################################################

MESSAGE("Searching for system CLANG libraries")
FIND_LIBRARY(clangFrontend_LIBRARY NAMES  clangFrontend DOC "Location of the clangFrontend library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangSerialization_LIBRARY NAMES clangSerialization DOC "Location of the clangSerialization library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangDriver_LIBRARY NAMES clangDriver DOC "Location of the clangDriver library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangCodeGen_LIBRARY NAMES clangCodeGen DOC "Location of the clangCodeGen library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangSema_LIBRARY NAMES clangSema DOC "Location of the clangSema library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangStaticAnalyzerFrontend_LIBRARY NAMES clangStaticAnalyzerFrontend DOC "Location of the clangStaticAnalyzerFrontend library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangStaticAnalyzerCheckers_LIBRARY NAMES clangStaticAnalyzerCheckers DOC "Location of the clangStaticAnalyzerCheckers library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangStaticAnalyzerCore_LIBRARY NAMES clangStaticAnalyzerCore DOC "Location of the clangStaticAnalyzerCore library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangAnalysis_LIBRARY NAMES clangAnalysis DOC "Location of the clangAnalysis library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangRewriteCore_LIBRARY NAMES clangRewriteCore DOC "Location of the clangRewriteCore library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangRewriteFrontend_LIBRARY NAMES clangRewriteFrontend DOC "Location of the clangRewriteFrontend library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangAST_LIBRARY NAMES clangAST DOC "Location of the clangAST library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangParse_LIBRARY NAMES clangParse DOC "Location of the clangParse library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangLex_LIBRARY NAMES clangLex DOC "Location of the clangLex library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangBasic_LIBRARY NAMES clangBasic DOC "Location of the clangBasic library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangEdit_LIBRARY NAMES clangEdit DOC "Location of the clangEdit library" PATHS ${LOCAL_PATH})
FIND_LIBRARY(clangARCMigrate_LIBRARY NAMES clangARCMigrate DOC "Location of the clangARCMigrate library" PATHS ${LOCAL_PATH})


if ( clangFrontend_LIBRARY AND
    clangSerialization_LIBRARY AND
    clangDriver_LIBRARY AND
    clangCodeGen_LIBRARY AND
    clangSema_LIBRARY AND
    clangStaticAnalyzerFrontend_LIBRARY AND
    clangStaticAnalyzerCheckers_LIBRARY AND
    clangStaticAnalyzerCore_LIBRARY AND
    clangAnalysis_LIBRARY AND
    clangRewriteCore_LIBRARY AND
    clangRewriteFrontend_LIBRARY AND
    clangAST_LIBRARY AND
    clangParse_LIBRARY AND
    clangLex_LIBRARY AND
    clangBasic_LIBRARY AND
    clangEdit_LIBRARY AND
    clangARCMigrate_LIBRARY
)
  set(CLANG_FOUND TRUE)
  set(CLANG_LIBRARIES   ${clangFrontend_LIBRARY}  ${clangSerialization_LIBRARY}  ${clangDriver_LIBRARY} 
    ${clangCodeGen_LIBRARY}  ${clangStaticAnalyzerFrontend_LIBRARY}
    ${clangStaticAnalyzerCheckers_LIBRARY}  ${clangStaticAnalyzerCore_LIBRARY}  ${clangAnalysis_LIBRARY}
    ${clangRewriteCore_LIBRARY}  ${clangRewriteFrontend_LIBRARY}  ${clangAST_LIBRARY}  ${clangParse_LIBRARY}
    ${clangSema_LIBRARY}  ${clangLex_LIBRARY}  ${clangBasic_LIBRARY} ${clangEdit_LIBRARY} ${clangARCMigrate_LIBRARY})
  set(CLANG_LIBRARIES ${CLANG_LIBRARIES} ${CLANG_LIBRARIES})
  MESSAGE("Found CLANG libraries")
else()
  set(CLANG_FOUND FALSE)
  MESSAGE("Did not find CLANG libraries")
endif()


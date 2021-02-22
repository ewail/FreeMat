ENABLE_TESTING()
ADD_TEST(wbtest_symvar_1 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/function" "-f" "wb_test('@$exact#y1=symvar(x1)','wbtest_symvar_1','${CMAKE_SOURCE_DIR}/tests/reference')")

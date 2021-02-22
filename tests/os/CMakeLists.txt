ENABLE_TESTING()
ADD_TEST(wbtest_fileparts_1 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/os" "-f" "wb_test('@$exact#[y1,y2,y3,y4]=fileparts(''/home/basu/testfile.mgz'')','wbtest_fileparts_1','${CMAKE_SOURCE_DIR}/tests/reference')")
